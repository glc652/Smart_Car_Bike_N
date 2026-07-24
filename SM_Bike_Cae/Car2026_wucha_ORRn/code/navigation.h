/*
 * navigation.h
 *
 * 循迹路径记录与回放
 *
 * 设计思路参考开源项目 navigation-master:
 *   - 每行驶固定距离采样一次偏航角存入Flash
 *   - 回放时按同样距离读取偏航角作为转向目标
 *
 * Flash页分配:
 *   第7页 : 元数据页 (存总点数)
 *   第8~10页: 路径数据 (每页200点, 共3页 = 600点)
 *
 * 采样间隔: 每 NAG_SAMPLE_DIST 记录一次 (默认0.05m = 5cm)
 *
 * Created on: 2026-06-03
 */

#ifndef CODE_NAVIGATION_H_
#define CODE_NAVIGATION_H_

#include "zf_common_typedef.h"

//===================================================配置参数===================================================

// Flash页布局
#define NAG_META_PAGE           7       // 元数据页 (存总点数)
#define NAG_DATA_START_PAGE     8       // 数据起始页
#define NAG_DATA_END_PAGE       10      // 数据结束页 (含)
#define NAG_MAX_PAGES           (NAG_DATA_END_PAGE - NAG_DATA_START_PAGE + 1)  // 3页
#define NAG_POINTS_PER_PAGE     200     // 每页最多200个数据点
#define NAG_MAX_POINTS          (NAG_MAX_PAGES * NAG_POINTS_PER_PAGE)  // 600点

// 采样参数
#define NAG_SAMPLE_DIST         0.05f   // 每5cm采样一次偏航角 (单位m)
#define NAG_YAW_SCALE           100     // 偏航角放大倍数 (float转int, 精度0.01度)

// 前瞻点数 (读取时预加载后面点, 用于Pure Pursuit前瞻控制)
#define NAG_LOOKAHEAD           5

//===================================================系统状态===================================================

typedef enum {
    NAG_STATE_IDLE    = 0,  // 空闲 (未启动)
    NAG_STATE_RECORD  = 1,  // 记录模式: 行驶同时保存路径
    NAG_STATE_REPLAY  = 3,  // 回放模式: 按记录轨迹行驶
} Nag_State_t;

//===================================================控制结构===================================================

typedef struct {
    Nag_State_t state;          // 当前状态
    uint16      total_points;   // 已记录总数据点数
    uint16      record_index;   // 记录时: 当前写入位置
    uint16      replay_index;   // 回放时: 当前读取位置
    float       dist_accum;     // 累计行驶距离 (m), 到达NAG_SAMPLE_DIST时采样
    float       final_yaw_out;  // 输出目标偏航角 (上层控制使用)
    uint8       page_index;     // 当前使用的Flash页号
    uint8       page_offset;    // 当前页内写入偏移
    uint8       end_flag;       // 回放是否到达终点
} Nag_Ctrl_t;

extern Nag_Ctrl_t Nag;

//===================================================外部接口===================================================

// 初始化导航模块
void Navigation_Init(void);

// 开始记录 (每行驶距离保存一个点)
void Navigation_Start_Record(void);

// 停止记录并将剩余缓存写入Flash (最后一个点必须调用)
void Navigation_Stop_Record(void);

// 开始回放 (上电后首次开始时调用)
void Navigation_Start_Replay(void);

// 停止回放
void Navigation_Stop_Replay(void);

// 更新里程 + 记录/读取 (5ms中断或Ctrl_main调用, 每帧传入本帧行驶距离)
void Navigation_Update(float delta_dist);

// 获取输出偏航角 (给yaw_control使用, 回放时有效)
float Navigation_Get_Yaw_Output(void);

// 查看当前状态
Nag_State_t Navigation_Get_State(void);

// 查看是否回放结束
uint8 Navigation_Is_End(void);

// 查看已记录点数
uint16 Navigation_Get_Total_Points(void);

// 擦除所有导航Flash数据
void Navigation_Erase_All(void);

// 获取当前实时偏航角, 仅记录显示使用
float Navigation_Get_Current_Yaw(void);

// Print latest recorded XY coordinate in cm from main loop.
void Navigation_Print_XY(void);

#endif /* CODE_NAVIGATION_H_ */
