/*
 * navigation.c
 *
 * 循迹路径记录与回放
 *
 * 工作机制 (参考 navigation-master/tc):
 *   记录模式:
 *     - 累计行驶距离, 每 NAG_SAMPLE_DIST(5cm) 保存一次当前偏航角
 *     - 偏航角 * NAG_YAW_SCALE 转 int32 存入Flash缓存
 *     - 满500点写入Flash, 换页继续
 *
 *   回放模式:
 *     - 同样每 NAG_SAMPLE_DIST 触发
 *     - 从Flash读取对应偏航角
 *     - final_yaw_out = 记录时的偏航角 (目标角度), 直接传给 yaw_control
 *
 * Flash存储说明:
 *   flash_union_buffer 每单元使用 flash_data_union (4字节), 成员:
 *     .int32_type   / .uint32_type / .float_type / .int16_type / .uint16_type / ...
 *   注意: uint16_type 数组是 uint16, 占半字, 读取时取 [0][1] 组合。
 *   本文件统一使用 .int32_type (存偏航角) 和 .uint32_type (存单元数/有效点数)。
 *
 * Created on: 2026-06-03
 */

#include "navigation.h"
#include "Imu_Enhanced.h"
#include "zf_driver_flash.h"
#include "zf_common_headfile.h"
#include "stdio.h"
#include "math.h"

//===================================================参数定义===================================================

// 缓存区 (直接映射 navigation_union_buffer, 避免多任务写Flash冲突)
static int32 nag_write_buf[NAG_POINTS_PER_PAGE];
static uint16 nag_buf_count = 0;        // 当前缓存有效数据个数

// 回放时当前页缓存
static int32 nag_read_buf[NAG_POINTS_PER_PAGE];
static uint8  nag_read_page = NAG_DATA_START_PAGE;
static uint16 nag_read_page_valid = 0;  // 当前读取页有效点数

static float nag_x_cm = 0.0f;
static float nag_y_cm = 0.0f;
static float nag_print_x_cm = 0.0f;
static float nag_print_y_cm = 0.0f;
static float nag_print_yaw = 0.0f;
static uint16 nag_print_index = 0;
static volatile uint8 nag_xy_pending = 0;

//===================================================全局状态===================================================

Nag_Ctrl_t Nag = {
    .state         = NAG_STATE_IDLE,
    .total_points  = 0,
    .record_index  = 0,
    .replay_index  = 0,
    .dist_accum    = 0.0f,
    .final_yaw_out = 0.0f,
    .page_index    = NAG_DATA_START_PAGE,
    .page_offset   = 0,
    .end_flag      = 0,
};

//===================================================内部函数===================================================

static void nag_reset_xy(void)
{
    nag_x_cm = 0.0f;
    nag_y_cm = 0.0f;
    nag_print_x_cm = 0.0f;
    nag_print_y_cm = 0.0f;
    nag_print_yaw = 0.0f;
    nag_print_index = 0;
    nag_xy_pending = 0;
}

static void nag_update_xy_from_segment(float segment_dist_m, float yaw_deg)
{
    const float deg_to_rad = 0.01745329252f;
    float dist_cm = segment_dist_m * 100.0f;
    float yaw_rad = yaw_deg * deg_to_rad;

    nag_x_cm += dist_cm * sinf(yaw_rad);
    nag_y_cm += dist_cm * cosf(yaw_rad);

    nag_print_x_cm = nag_x_cm;
    nag_print_y_cm = nag_y_cm;
    nag_print_yaw = yaw_deg;
    nag_print_index = Nag.record_index;
    nag_xy_pending = 1;
}

/*
 * 将缓存 nag_write_buf[0..nag_buf_count-1] 写入 Flash 指定 page 页
 * 格式:
 *   [0..nag_buf_count-1] : int32 偏航角数据
 *   [NAG_POINTS_PER_PAGE]: uint32 本页有效点数 (放在501号位, 1024字节足够)
 */
static void nag_flush_page(uint8 page)
{
    uint16 i;
    flash_buffer_clear();
    for (i = 0; i < nag_buf_count; i++) {
        flash_union_buffer[i].int32_type = nag_write_buf[i];
    }
    flash_union_buffer[NAG_POINTS_PER_PAGE].uint32_type = (uint32)nag_buf_count;

    flash_erase_page(0, page);
    flash_write_page_from_buffer(0, page);
}

/*
 * 元数据 (总点数 + 取反校验) 写入元数据页
 * 格式:
 *   [0]: uint32 = total_points
 *   [1]: uint32 = ~total_points (校验)
 */
static void nag_save_meta(void)
{
    flash_buffer_clear();
    flash_union_buffer[0].uint32_type = (uint32)Nag.total_points;
    flash_union_buffer[1].uint32_type = (uint32)(~Nag.total_points);
    flash_erase_page(0, NAG_META_PAGE);
    flash_write_page_from_buffer(0, NAG_META_PAGE);
}

/*
 * 读取元数据, 成功1 失败0 (校验不通过或未写入)
 */
static uint8 nag_load_meta(void)
{
    uint32 total, check;
    flash_read_page_to_buffer(0, NAG_META_PAGE);
    total = flash_union_buffer[0].uint32_type;
    check = flash_union_buffer[1].uint32_type;
    if ((uint32)(~total) != check) return 0;
    if (total > NAG_MAX_POINTS) return 0;
    Nag.total_points = (uint16)total;
    return 1;
}

/*
 * 读取指定页到 nag_read_buf, 返回本页有效点数
 */
static uint16 nag_load_page(uint8 page)
{
    uint16 i, valid;
    flash_read_page_to_buffer(0, page);
    valid = (uint16)flash_union_buffer[NAG_POINTS_PER_PAGE].uint32_type;
    if (valid > NAG_POINTS_PER_PAGE) valid = NAG_POINTS_PER_PAGE;
    for (i = 0; i < valid; i++) {
        nag_read_buf[i] = flash_union_buffer[i].int32_type;
    }
    return valid;
}

/*
 * 将当前偏航角写入缓存, 满页刷新到Flash
 */
static void nag_record_sample(float yaw)
{
    if (Nag.total_points >= NAG_MAX_POINTS) {
        Navigation_Stop_Record();
        return;
    }
    nag_write_buf[nag_buf_count] = (int32)(yaw * NAG_YAW_SCALE);
    nag_buf_count++;
    Nag.total_points++;
    Nag.record_index++;

    if (nag_buf_count >= NAG_POINTS_PER_PAGE) {
        if (Nag.page_index <= NAG_DATA_END_PAGE) {
            nag_flush_page(Nag.page_index);
            Nag.page_index++;
        } else {
            // 空间满, 停止记录
            Navigation_Stop_Record();
            return;
        }
        nag_buf_count = 0;
    }
}

/*
 * 获取回放模式下第 idx 个点的偏航角 (单位度)
 * 自动分页加载Flash, 减少Flash读取次数
 */
static float nag_replay_get_yaw(uint16 idx)
{
    uint8  target_page    = (uint8)(NAG_DATA_START_PAGE + idx / NAG_POINTS_PER_PAGE);
    uint16 offset_in_page = idx % NAG_POINTS_PER_PAGE;

    if (target_page > NAG_DATA_END_PAGE) {
        return 0.0f;
    }

    if (target_page != nag_read_page || nag_read_page_valid == 0) {
        nag_read_page       = target_page;
        nag_read_page_valid = nag_load_page(target_page);
    }

    if (offset_in_page >= nag_read_page_valid) {
        return 0.0f;  // 越界保护
    }

    return (float)nag_read_buf[offset_in_page] / (float)NAG_YAW_SCALE;
}

//===================================================外部接口===================================================

void Navigation_Init(void)
{
    Nag.state         = NAG_STATE_IDLE;
    Nag.total_points  = 0;
    Nag.record_index  = 0;
    Nag.replay_index  = 0;
    Nag.dist_accum    = 0.0f;
    Nag.final_yaw_out = 0.0f;
    Nag.page_index    = NAG_DATA_START_PAGE;
    Nag.page_offset   = 0;
    Nag.end_flag      = 0;

    nag_buf_count       = 0;
    nag_read_page       = NAG_DATA_START_PAGE;
    nag_read_page_valid = 0;
    nag_reset_xy();
}

void Navigation_Start_Record(void)
{
    Navigation_Init();
    Gyro_Reset_Attitude();
    Nag.state = NAG_STATE_RECORD;
//    printf("NAG: Start record\n");
}

void Navigation_Stop_Record(void)
{
    if (Nag.state != NAG_STATE_RECORD) return;

    // 剩余缓存写入Flash
    if (nag_buf_count > 0 && Nag.page_index <= NAG_DATA_END_PAGE) {
        nag_flush_page(Nag.page_index);
        Nag.page_index++;
    }

    nag_save_meta();
    Nag.state = NAG_STATE_IDLE;
//    printf("NAG: Stop record, total=%d points\n", Nag.total_points);
}

void Navigation_Start_Replay(void)
{
    if (Nag.state == NAG_STATE_RECORD) {
        Navigation_Stop_Record();
    }

    if (!nag_load_meta() || Nag.total_points == 0) {
        printf("NAG: No valid record, replay aborted\n");
        return;
    }

    Nag.state        = NAG_STATE_REPLAY;
    Nag.replay_index = 0;
    Nag.dist_accum   = 0.0f;
    Nag.end_flag     = 0;
    Gyro_Reset_Attitude();
    Control_Reset_For_Navigation();
    // 预加载第一页
    nag_read_page       = NAG_DATA_START_PAGE;
    nag_read_page_valid = nag_load_page(NAG_DATA_START_PAGE);

    // 取第0点作为初始偏航目标
    if (Nag.total_points > 0) {
        Nag.final_yaw_out = nag_replay_get_yaw(0);
    }

    printf("NAG: Start replay, total=%d points\n", Nag.total_points);
}

void Navigation_Stop_Replay(void)
{
    Nag.state    = NAG_STATE_IDLE;
    Nag.end_flag = 1;
    Control_Reset_For_Navigation();
}

void Navigation_Update(float delta_dist)
{
    if (Nag.state == NAG_STATE_IDLE) return;

    Nag.dist_accum += delta_dist;

    while (Nag.dist_accum >= NAG_SAMPLE_DIST) {
        Nag.dist_accum -= NAG_SAMPLE_DIST;

        if (Nag.state == NAG_STATE_RECORD) {
            if (Nag.total_points >= NAG_MAX_POINTS) {
                Navigation_Stop_Record();
                return;
            }

            //解算成xy
            nag_update_xy_from_segment(NAG_SAMPLE_DIST, Gyro_YawAngle_Get());
            nag_record_sample(Gyro_YawAngle_Get());
        } else if (Nag.state == NAG_STATE_REPLAY) {
            Nag.replay_index++;
            if (Nag.replay_index >= Nag.total_points) {
                Nag.end_flag = 1;
                Nag.state    = NAG_STATE_IDLE;
                Control_Reset_For_Navigation();
//                printf("NAG: Replay end\n");
                return;
            }
            Nag.final_yaw_out = nag_replay_get_yaw(Nag.replay_index);
        }
    }
}

float Navigation_Get_Yaw_Output(void)
{
    return Nag.final_yaw_out;
}

Nag_State_t Navigation_Get_State(void)
{
    return Nag.state;
}

uint8 Navigation_Is_End(void)
{
    return Nag.end_flag;
}

uint16 Navigation_Get_Total_Points(void)
{
    return Nag.total_points;
}

void Navigation_Erase_All(void)
{
    uint8 p;
    flash_erase_page(0, NAG_META_PAGE);
    for (p = NAG_DATA_START_PAGE; p <= NAG_DATA_END_PAGE; p++) {
        flash_erase_page(0, p);
    }
    Navigation_Init();
    printf("NAG: All flash erased\n");
}

float Navigation_Get_Current_Yaw(void)
{
    return Gyro_YawAngle_Get();
}

void Navigation_Print_XY(void)
{
    if (!nag_xy_pending) {
        return;
    }

    nag_xy_pending = 0;
    printf("%.2f,%.2f\r\n",nag_print_x_cm, nag_print_y_cm);
//    printf("NAG_XY idx=%d yaw=%.2f x=%.2fcm y=%.2fcm\r\n",
//           nag_print_index, nag_print_yaw, nag_print_x_cm, nag_print_y_cm);
}
