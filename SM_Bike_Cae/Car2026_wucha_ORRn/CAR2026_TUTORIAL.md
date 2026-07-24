# Car2026 自平衡单车惯性导航系统 教程

> 平台: AURIX TC264D | IDE: ADS v1.10.2 | 编译器: Tasking

---

## 目录

1. [系统概述](#1-系统概述)
2. [硬件参数](#2-硬件参数)
3. [软件架构](#3-软件架构)
4. [惯性导航原理](#4-惯性导航原理)
5. [Flash 存储布局](#5-flash-存储布局)
6. [代码模块详解](#6-代码模块详解)
7. [调用链分析](#7-调用链分析)
8. [菜单操作说明](#8-菜单操作说明)
9. [使用流程（比赛）](#9-使用流程比赛)
10. [参数调整](#10-参数调整)
11. [常见问题](#11-常见问题)

---

## 1. 系统概述

Car2026 是基于 TC264 的自平衡两轮单车，具备以下能力：

| 功能 | 说明 |
|------|------|
| 自平衡 | Pitch 角 PID 闭环，保持车身直立 |
| 速度控制 | 编码器反馈，增量式 PID |
| 偏航控制 | Yaw 角 PID 闭环，控制方向 |
| PS2 手动驾驶 | 摇杆控制速度和偏航目标 |
| **惯性导航** | **记录第一圈路径，第二圈自动重放** |
| PID 调参 | 菜单 + 按键实时调整，掉电保存 |

### 惯性导航的核心思想

```
第一圈（手动）:  每走 5cm → 记录当前偏航角 → 存入 Flash
第二圈（自动）:  每走 5cm → 从 Flash 读取对应偏航角 → 作为转向目标
```

这样第二圈就能"回放"第一圈的路径，实现自动驾驶。

---

## 2. 硬件参数

| 参数 | 数值 |
|------|------|
| 轮子半径 | 32 mm（0.032 m）|
| 轴距 | 205 mm（0.205 m）|
| 编码器分辨率 | 4100 脉冲/圈 |
| IMU | ICM-42688（imu660rb）|
| 显示屏 | IPS200 SPI 接口 |
| 按键 | KEY1~KEY4（GPIO 低电平有效）|
| PS2 摇杆 | 手柄控制 |

---

## 3. 软件架构

### 多速率控制结构

```
1ms  中断 (CCU60_CH0)
├── gyro_y_control()     ← 角速度内环（最快，稳定直立）
│
├── 5ms 倍频
│   ├── roll_control()   ← 角度外环（平衡）
│   ├── Ctrl_main()      ← 速度/电机输出
│   └── Navigation_Update(delta)  ← 导航里程累积
│
└── 20ms 倍频
    └── yaw_control(yaw_target)   ← 偏航控制
        │
        ├── 导航模式: yaw_target = Navigation_Get_Yaw_Output()
        └── 手动模式: yaw_target = PS2_Get_Yaw_Target()

主循环 (while)
├── car_safety()          ← 倾倒保护
├── PID_Menu_Key_Process()← 按键处理（10ms 节奏）
├── PID_Tuning_Menu_Show()← 屏幕刷新
└── PS2_Control_Process() ← 摇杆处理
```

### 文件结构

```
Car2026/
├── user/
│   └── cpu0_main.c         ← 主程序、中断、初始化
└── code/
    ├── navigation.h/.c     ← 惯性导航模块（新增）
    ├── navigation.h        ← 接口定义、宏、状态枚举
    ├── Menu_PID_Tuning.h/.c← PID调参菜单（已扩展导航子菜单）
    ├── Car_control.h/.c    ← 三环 PID 控制
    ├── encode.h/.c         ← 编码器里程计
    ├── Imu_Enhanced.h/.c   ← IMU 姿态解算（四元数 AHRS）
    ├── PID_Flash_Storage.h/.c ← PID 参数 Flash 存储
    └── ps2_control.h/.c    ← PS2 摇杆控制
```

---

## 4. 惯性导航原理

### 4.1 偏航角来源

偏航角由 `Imu_Enhanced.c` 中的四元数 Madgwick 滤波器持续计算：

```
IMU 原始数据（加速度 + 陀螺仪）
        ↓
  Madgwick 算法（400Hz 更新）
        ↓
  四元数 → 欧拉角
        ↓
  Gyro_YawAngle_Get()  ← navigation.c 调用此函数
```

Yaw 角是**连续累积量**，不会在 ±180° 处跳变（代码有连续性处理）。

### 4.2 里程计来源

```
编码器脉冲 → Get_Encoder_Pulses()
         ↓
  Encoder_Distance (全局, 单位 m)
  = Σ (脉冲数 / 4100 × 2π × 0.032)
```

每 5ms 在中断中计算本次增量 `delta`，传入 `Navigation_Update(delta)`。

### 4.3 记录过程

```
Navigation_Update(delta_dist) 被调用
    ↓
dist_accum += delta_dist
    ↓
while dist_accum >= 0.05m:
    dist_accum -= 0.05m
    nag_record_sample()
        ↓
        yaw = Gyro_YawAngle_Get()
        nag_write_buf[count++] = (int32)(yaw × 100)
        
        若 count == 500:
            nag_flush_page(当前页)   ← 写入 Flash
            页号++, count = 0
```

### 4.4 重放过程

```
Navigation_Update(delta_dist) 被调用
    ↓
同样的里程计触发
    ↓
replay_index++
final_yaw_out = nag_replay_get_yaw(replay_index)
    ↓
    计算目标页 = 13 + index / 500
    若页未缓存 → 从 Flash 加载该页到 nag_read_buf[]
    return nag_read_buf[index % 500] / 100.0f
```

重放输出的角度直接作为 `yaw_control()` 的目标值。

### 4.5 精度说明

| 指标 | 数值 |
|------|------|
| 采样间距 | 5 cm |
| 角度精度 | 0.01°（× 100 缩放后存 int32）|
| 最大路径长度 | 600 点 × 0.05m = **30 m** |
| Flash 占用 | 第 7 页（元数据）+ 第 8~10 页（数据，共 4 页）|

---

## 5. Flash 存储布局

TC264 内部 Flash，每页 1KB（256 个 32bit 字）。

```
页号    内容
─────────────────────────────────────────────
  11    PID 参数（PID_Flash_Storage 使用）
─────────────────────────────────────────────
   7    导航元数据页
        [0]  uint32 = total_points（总记录点数）
        [1]  uint32 = ~total_points（反码校验）
─────────────────────────────────────────────
   8    第 0 批数据（点 0~199）
        [0..199]  int32 = yaw × 100
        [200]     uint32 = 本页有效点数
─────────────────────────────────────────────
   9    第 1 批数据（点 200~399）
  10    第 2 批数据（点 400~599）
─────────────────────────────────────────────
```

每页 1KB = 256 × 4 字节，200 数据点 + 1 有效点数 = 201 字，安全余量 55 字。

`flash_data_union` 是 **4 字节单值联合体**，访问方式：
- `flash_union_buffer[i].int32_type`  ← 偏航角数据
- `flash_union_buffer[i].uint32_type` ← 计数/校验值
- **不可以** 用下标 `[i].uint16_type[0]`（uint16_type 是标量）

---

## 6. 代码模块详解

### navigation.h — 接口与配置

```c
// Flash 页分配
#define NAG_META_PAGE       12    // 元数据
#define NAG_DATA_START_PAGE 13    // 数据起始
#define NAG_DATA_END_PAGE   30    // 数据结束

// 采样参数
#define NAG_SAMPLE_DIST     0.05f // 每 5cm 采样一次
#define NAG_YAW_SCALE       100   // float→int 缩放

// 状态枚举
typedef enum {
    NAG_STATE_IDLE   = 0,  // 待机
    NAG_STATE_RECORD = 1,  // 记录中
    NAG_STATE_REPLAY = 3,  // 重放中
} Nag_State_t;

// 控制结构体（全局可见 extern Nag_Ctrl_t Nag）
typedef struct {
    Nag_State_t state;
    uint16 total_points;   // 总记录点数
    uint16 replay_index;   // 重放进度
    float  dist_accum;     // 里程累积（触发采样用）
    float  final_yaw_out;  // 当前输出的偏航目标
    uint8  end_flag;       // 重放是否结束
    ...
} Nag_Ctrl_t;
```

### navigation.c — 核心逻辑

| 函数 | 说明 |
|------|------|
| `Navigation_Init()` | 清零所有状态，不操作 Flash |
| `Navigation_Start_Record()` | 调用 Init，设置 state = RECORD |
| `Navigation_Stop_Record()` | 刷写剩余缓冲，保存元数据，state = IDLE |
| `Navigation_Start_Replay()` | 读取元数据，预加载第一页，state = REPLAY |
| `Navigation_Stop_Replay()` | 强制 state = IDLE |
| `Navigation_Update(delta)` | **中断中调用**，里程触发采样/重放 |
| `Navigation_Get_Yaw_Output()` | 返回 `final_yaw_out`，供偏航控制使用 |
| `Navigation_Erase_All()` | 擦除第 12~30 页全部数据 |

### cpu0_main.c — 集成要点

```c
// 初始化（Code_Init 中）
Navigation_Init();

// 5ms 中断：里程更新
static float nag_last_dist = 0.0f;
float delta = Encoder_Distance - nag_last_dist;
if (delta < 0.0f) delta = -delta;   // 倒退也计距离
nag_last_dist = Encoder_Distance;
Navigation_Update(delta);

// 20ms 中断：偏航目标选择
float yaw_target = (Navigation_Get_State() == NAG_STATE_REPLAY)
                   ? Navigation_Get_Yaw_Output()   // 导航模式
                   : PS2_Get_Yaw_Target();          // 手动模式
yaw_put = yaw_control(yaw_target);
```

---

## 7. 调用链分析

### 记录一圈完整调用链

```
[比赛场地就位] 菜单 → Navigation → Start Record
                          │
                          ▼
               Navigation_Start_Record()
               └── Navigation_Init()     ← 清零状态
               └── Nag.state = RECORD

[开始手动驾驶，1ms 中断持续运行]

每 5ms:
  Get_Encoder_Pulses()
    └── Encoder_Distance += 本次里程
  
  delta = Encoder_Distance - nag_last_dist
  Navigation_Update(delta)
    └── Nag.dist_accum += delta
    └── while dist_accum >= 0.05f:
          dist_accum -= 0.05f
          nag_record_sample()
            └── yaw = Gyro_YawAngle_Get()
            └── nag_write_buf[count] = (int32)(yaw * 100)
            └── count++, total_points++
            └── if count == 500:
                  nag_flush_page(page_index)
                    └── flash_buffer_clear()
                    └── flash_union_buffer[0..499].int32_type = nag_write_buf[0..499]
                    └── flash_union_buffer[500].uint32_type = 500
                    └── flash_erase_page(0, page)
                    └── flash_write_page_from_buffer(0, page)
                  page_index++, count = 0

[第一圈结束] 菜单 → Navigation → Stop Record
                          │
                          ▼
               Navigation_Stop_Record()
               └── nag_flush_page(最后一页)   ← 写入剩余数据
               └── nag_save_meta()
                    └── flash_union_buffer[0] = total_points
                    └── flash_union_buffer[1] = ~total_points
                    └── 写入第 12 页
               └── Nag.state = IDLE
```

### 重放一圈完整调用链

```
[第二圈就位] 菜单 → Navigation → Start Replay
                          │
                          ▼
               Navigation_Start_Replay()
               └── nag_load_meta()         ← 读第 12 页，校验
               └── Nag.total_points = 读出值
               └── nag_load_page(13)       ← 预加载第一页到 nag_read_buf[]
               └── Nag.replay_index = 0
               └── Nag.state = REPLAY
               └── final_yaw_out = nag_replay_get_yaw(0)

[第二圈自动驾驶]

每 5ms:
  Navigation_Update(delta)
    └── dist_accum >= 0.05f 时:
          replay_index++
          final_yaw_out = nag_replay_get_yaw(replay_index)
            └── target_page = 13 + index / 500
            └── 若页不同于已加载页:
                  nag_load_page(target_page)  ← 换页
            └── return nag_read_buf[index % 500] / 100.0f

每 20ms:
  yaw_target = Navigation_Get_Yaw_Output()  ← 返回 final_yaw_out
  yaw_put = yaw_control(yaw_target)
    └── PID 计算偏航力矩
    └── 差速输出给电机
```

---

## 8. 菜单操作说明

### 按键定义

| 按键 | GPIO | 功能 |
|------|------|------|
| KEY1 | UP | 向上移动光标 |
| KEY2 | DOWN | 向下移动光标 |
| KEY3 | SELECT | 进入/执行 |
| KEY4 | BACK | 短按减值/返回；长按（500ms）强制返回上级 |

### 菜单树

```
主菜单（SELECT 进入）
├── Speed     → Speed PID 调参（Kp/Ki/Kd）
├── Angle     → 平衡角 PID 调参
├── Gyro      → 角速度 PID 调参
├── Yaw       → 偏航 PID 调参
│    └── 进入后 SELECT: +step  BACK: -step  UP/DOWN: 调步长  长按BACK: 返回
├── Flash     → Flash 操作
│    ├── Save PID      ← 保存所有 PID 到 Flash
│    ├── Load PID      ← 从 Flash 读取 PID
│    ├── Erase Flash   ← 擦除 PID 数据页
│    └── View Status   ← 查看 Flash 状态
└── Navigation → 导航操作（新增）
     ├── Start Record  ← 开始记录（第一圈前按）
     ├── Stop  Record  ← 停止记录（第一圈结束按）
     ├── Start Replay  ← 开始重放（第二圈前按）
     ├── Stop  Replay  ← 中途强制停止
     └── Erase Data    ← 清除所有导航数据
```

### 导航子菜单屏幕显示

```
Navigation
State: IDLE / RECORDING / REPLAYING
Points:  1234
Dist:  61.7m

  Start Record
  Stop  Record
> Start Replay     ← 光标
  Stop  Replay
  Erase Data

SELECT:exec BACK:ret
```

---

## 9. 使用流程（比赛）

### 第一圈：记录路径

1. 车辆上电，系统初始化
2. **菜单** → `Navigation` → `Start Record`（SELECT 执行）
3. 使用 PS2 摇杆**手动驾驶**完整跑一圈
4. 回到起点后 **菜单** → `Navigation` → `Stop Record`
5. 屏幕显示 `Points: XXXX`，确认记录成功

### 第二圈：自动重放

1. 车辆回到起点，姿态与第一圈起始一致
2. **菜单** → `Navigation` → `Start Replay`（SELECT 执行）
3. 松开 PS2 摇杆（或不操作），车辆将自动跟随记录的偏航角轨迹
4. 重放结束后自动切换回 IDLE，可通过 PS2 接管控制
5. 如需重新记录，先 `Erase Data` 再重复步骤

### 注意事项

- 每次开机后 IMU 需要**静止约 2 秒**完成陀螺仪零偏校准
- 重放时的**起始方向**必须与记录时一致，否则路径偏差累积
- 记录时如果路径超过 450m（9000 点），会自动停止记录
- `Erase Data` 会擦除 19 页 Flash（约需 1~2 秒），操作期间不要断电

---

## 10. 参数调整

### 导航相关参数（navigation.h）

| 宏 | 默认值 | 说明 |
|----|--------|------|
| `NAG_META_PAGE` | 7 | 元数据页号 |
| `NAG_DATA_START_PAGE` | 8 | 数据起始页 |
| `NAG_DATA_END_PAGE` | 10 | 数据结束页 |
| `NAG_SAMPLE_DIST` | 0.05f | 采样间距（m），越小越精确，占用 Flash 越多 |
| `NAG_POINTS_PER_PAGE` | 200 | 每页存储点数（每页 1KB，200点安全余量充足）|
| `NAG_YAW_SCALE` | 100 | 角度精度，100 = 0.01° |

### 编码器参数（encode.c）

| 宏 | 当前值 | 说明 |
|----|--------|------|
| `WHEEL_RADUS` | 0.032f | 轮子半径，影响里程精度 |
| `PULSES_PER_REV` | 4100 | 编码器分辨率，出厂固定 |
| `DT` | 0.005f | 控制周期，与中断频率一致 |

### PID 参数（菜单调整，掉电保存）

| PID | 作用 | 调整方法 |
|-----|------|---------|
| Speed | 速度闭环 | Kp 影响响应速度，Ki 消稳态误差 |
| Angle | 平衡角环 | Kp 过大→震荡，过小→倒车 |
| Gyro | 角速度阻尼 | Kp 提高稳定性，过大→高频震荡 |
| Yaw | 偏航方向 | Kp 影响转向灵敏度 |

---

## 11. 常见问题

**Q: 重放时车辆方向偏差越来越大**

A: 原因通常有两个：
1. 重放起始方向与记录时不一致 → 每次从相同方向出发
2. IMU 零偏未充分校准 → 上电后静止等待 2 秒以上

**Q: Start Replay 后屏幕提示但车辆不动**

A: 导航元数据校验失败（`total_points == 0` 或校验不通过）。先 `Erase Data`，再重新 `Start Record`。

**Q: 编译报错 E244/E245（无效下标）**

A: `flash_data_union` 的各成员（`uint16_type`、`int32_type` 等）是**标量**，不是数组，不能用 `[0][1]` 下标。访问元数据应使用不同索引：`flash_union_buffer[0].uint32_type`、`flash_union_buffer[1].uint32_type`。

**Q: 菜单按键无响应**

A: `PID_Menu_Key_Process()` 在主循环中调用，去抖间隔依赖主循环速度。若主循环被阻塞（如大量 printf），按键会不灵敏。

**Q: 想调整采样间距到 2cm**

A: 将 `NAG_SAMPLE_DIST` 改为 `0.02f`，同时注意最大路径长度缩短为 `9000 × 0.02 = 180m`。

---

*文档对应代码版本: 2026-06-03 | 车辆: Car2026 | 平台: TC264D*
