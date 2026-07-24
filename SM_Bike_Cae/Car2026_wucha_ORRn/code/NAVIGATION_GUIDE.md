# Car2026 惯导路径记录与回放系统

> 文件: `navigation.c` / `navigation.h`
> 创建: 2026-06-03

---

## 一、这套系统解决什么问题

自平衡单车在比赛时需要自主循迹。传统做法依赖摄像头或线感器识别赛道，但这套系统走的是另一条路：

**"先跟着我走一圈，记住沿途的偏航角；第二圈按照记住的角度自己走。"**

这就是惯性导航（INS, Inertial Navigation System）的核心思想：不依赖外部传感器（摄像头、GPS），只用车身自带的 IMU（陀螺仪）和编码器，就能复现之前走过的轨迹。

---

## 二、核心原理

### 2.1 坐标表达：偏航角（Yaw）

偏航角描述车头朝向相对于初始方向的夹角，由 IMU 通过四元数 AHRS 算法持续积分计算得到。

```
车头朝北出发 → Yaw = 0°
向右转90°    → Yaw = 90°
继续右转90°  → Yaw = 180°
再右转90°    → Yaw = 270°
```

Yaw 角是连续累积的（不会在 ±180° 处跳变），因此它天然代表了车辆的"转向历史"。

### 2.2 里程触发采样

每行驶 **5cm**，采样一次当前偏航角。

```
里程累加器: dist_accum += 本周期行驶距离
当 dist_accum >= 0.05m:
    dist_accum -= 0.05m
    采样一次 Yaw 角
```

用里程触发而不是时间触发，是因为速度变化时时间触发会导致数据点密度不均匀。里程触发保证每个数据点对应固定的空间距离，回放时对齐是精确的。

### 2.3 记录阶段：数据压缩存储

偏航角是 float（4字节），存储时乘以 100 转为 int32，精度保留到 0.01°：

```c
存储: int32 = (int32)(yaw_float * 100)   // 例: 45.67° → 4567
读取: float = (float)int32_val / 100     // 4567 → 45.67°
```

500 个 int32 构成一页，每页写入一块 Flash。

### 2.4 回放阶段：偏航角重放

回放时，车辆同样用里程触发读取数据点，取出对应位置的目标偏航角，送给 `yaw_control()` 作为偏航目标：

```
第二圈每行驶5cm → 取出记录第N点的偏航角 → 作为yaw_control的目标
```

车辆的偏航 PID 会驱动舵机，使实际偏航角跟随目标偏航角，从而复现轨迹。

---

## 三、Flash 存储布局

```
Flash 页号      内容
─────────────────────────────────────────
  11            PID 参数 (PID_Flash_Storage, 原有，不动)
  12            导航元数据页 (总点数 + 校验)
  13            轨迹数据第1页 (500点, 对应 0~24.95m 路段)
  14            轨迹数据第2页 (500点, 对应 25~49.95m)
  ...
  30            轨迹数据第18页 (500点)
─────────────────────────────────────────
容量: 18页 × 500点 × 5cm/点 = 450m 轨迹
```

### 元数据页（第12页）格式

```
flash_union_buffer[0].uint16_type[0] = total_points  (总点数)
flash_union_buffer[0].uint16_type[1] = ~total_points (按位取反校验)
```

校验方式：读出后验证 `(uint16)(~total) == check`，如果不一致说明 Flash 数据无效（未写入或损坏）。

### 数据页（第13~30页）格式

```
flash_union_buffer[0..498].int32_type   = 偏航角数据 (×100的整数)
flash_union_buffer[500].uint16_type[0]  = 本页实际有效点数
```

最后一个槽存有效点数，是因为最后一页通常不满 500 点，需要记录实际写了多少。

---

## 四、代码结构

```
navigation.h          对外接口头文件
navigation.c
├── 静态变量
│   ├── nag_write_buf[500]      记录缓冲 (RAM)
│   ├── nag_read_buf[500]       回放缓冲 (RAM, 当前页的Flash数据)
│   └── nag_buf_count           当前缓冲已用数量
│
├── 内部函数 (static)
│   ├── nag_flush_page()        缓冲 → Flash 写一页
│   ├── nag_save_meta()         写元数据页
│   ├── nag_load_meta()         读元数据页 (含校验)
│   ├── nag_load_page()         Flash某页 → nag_read_buf
│   ├── nag_record_sample()     采样当前Yaw写入缓冲
│   └── nag_replay_get_yaw()    按序号取偏航角 (按需换页)
│
└── 外部接口
    ├── Navigation_Init()
    ├── Navigation_Start_Record()
    ├── Navigation_Stop_Record()
    ├── Navigation_Start_Replay()
    ├── Navigation_Stop_Replay()
    ├── Navigation_Update()          ← 主循环里每5ms调用
    ├── Navigation_Get_Yaw_Output()  ← 偏航控制读取
    ├── Navigation_Get_State()
    ├── Navigation_Is_End()
    ├── Navigation_Get_Total_Points()
    └── Navigation_Erase_All()
```

---

## 五、调用链逐步分析

### 5.1 系统启动

```
core0_main()
└── Code_Init()
    └── Navigation_Init()
```

`Navigation_Init()` 只做内存初始化，不读写 Flash：

```c
Nag.state        = NAG_STATE_IDLE;  // 空闲，什么都不做
Nag.total_points = 0;
Nag.dist_accum   = 0.0f;
nag_buf_count    = 0;
```

此时系统处于 IDLE，`Navigation_Update()` 被调用时直接 return，不消耗任何资源。

---

### 5.2 第一圈：记录模式

**触发时机：** 比赛开始，车辆出发前，调用：

```c
Navigation_Start_Record();
```

内部执行：
1. 先调 `Navigation_Init()` 清空所有状态
2. `Nag.state = NAG_STATE_RECORD`

**之后每 5ms 的中断（`cc60_pit_ch0_isr`）：**

```
1ms中断触发
├── gyro_y_control(angle_put)        角速度环
├── 5ms到了:
│   ├── angle_put = roll_control(yaw_put)   平衡控制
│   ├── Ctrl_main()                          读IMU + 编码器 + 控速
│   └── [导航里程更新]
│       ├── delta = Encoder_Distance - nag_last_dist   本周期行驶了多远
│       └── Navigation_Update(delta)
└── 20ms到了:
    ├── yaw_target = PS2_Get_Yaw_Target()   记录模式下用PS2控制
    └── yaw_put = yaw_control(yaw_target)
```

**`Navigation_Update(delta)` 内部（记录模式）：**

```c
Nag.dist_accum += delta;       // 累加里程

while (dist_accum >= 0.05m) {  // 每5cm触发一次
    dist_accum -= 0.05m;
    nag_record_sample();        // 采样当前Yaw
}
```

**`nag_record_sample()` 内部：**

```c
float yaw = Gyro_YawAngle_Get();           // 取当前偏航角
nag_write_buf[nag_buf_count] = (int32)(yaw * 100);  // 压缩存入缓冲
nag_buf_count++;
Nag.total_points++;

if (nag_buf_count >= 500) {
    nag_flush_page(Nag.page_index);  // 缓冲满了，写Flash
    Nag.page_index++;                // 翻到下一页
    nag_buf_count = 0;               // 缓冲清零
}
```

`nag_flush_page()` 操作：
```c
flash_buffer_clear();
// 把nag_write_buf拷贝进flash_union_buffer
for (i = 0; i < 500; i++)
    flash_union_buffer[i].int32_type = nag_write_buf[i];
flash_union_buffer[500].uint16_type[0] = 500;  // 记录本页有效点数

flash_erase_page(0, page);          // 擦除旧数据
flash_write_page_from_buffer(0, page);  // 写入
```

> **注意：** Flash 写操作（`nag_flush_page`）只在缓冲满 500 点时才发生，不是每 5cm 都写。写一次 Flash 耗时约 1~2ms，但因为每 500×5cm = 25m 才发生一次，不会影响实时控制。

**第一圈结束时：**

```c
Navigation_Stop_Record();
```

1. 把缓冲里剩余的不满 500 点也写入 Flash（最后一页）
2. 写元数据页（记录 total_points）
3. `Nag.state = NAG_STATE_IDLE`

---

### 5.3 第二圈：回放模式

**触发时机：** 车辆回到起点，调用：

```c
Navigation_Start_Replay();
```

内部执行：

1. 调 `nag_load_meta()` 从 Flash 第12页读 total_points，做校验
2. `Nag.state = NAG_STATE_REPLAY`
3. `Nag.replay_index = 0`（从第0个点开始）
4. 预加载第一页数据到 `nag_read_buf`
5. 取第0个点的偏航角作为初始目标：`Nag.final_yaw_out = 记录时的yaw[0]`

**之后每 5ms（同样的中断）：**

```
└── Navigation_Update(delta)
    Nag.dist_accum += delta;

    while (dist_accum >= 0.05m) {
        dist_accum -= 0.05m;
        Nag.replay_index++;                              // 指向下一个记录点
        Nag.final_yaw_out = nag_replay_get_yaw(index);  // 取目标偏航角
    }
```

**`nag_replay_get_yaw(idx)` 内部：**

```c
target_page    = 13 + idx / 500;  // 算出在哪一页
offset_in_page = idx % 500;       // 算出页内偏移

if (target_page != 当前缓存页) {
    nag_load_page(target_page);   // 按需从Flash加载新页到nag_read_buf
}

return (float)nag_read_buf[offset_in_page] / 100.0f;  // 还原偏航角
```

> **按需换页：** 绝大多数时候 replay_index 在当前页内递增，不需要读 Flash。只有越过每 25m 的边界时才触发一次换页读取，开销极低。

**偏航角目标如何影响车辆：**

```
20ms中断:
    yaw_target = Navigation_Get_Yaw_Output()   ← Nag.final_yaw_out
    yaw_put = yaw_control(yaw_target)
    
1ms中断:
    gyro_y_control(angle_put)
    angle_put = roll_control(yaw_put)
```

`yaw_control(yaw_target)` 计算误差 `= 目标Yaw - 当前Yaw`，通过 PID 输出舵机转角，驱动车辆朝记录时的方向行驶。

**回放结束：**

当 `Nag.replay_index >= Nag.total_points`，自动进入 IDLE，`end_flag = 1`。

---

## 六、完整调用时序图

```
上电
│
├─ Navigation_Init()              状态: IDLE
│
├─ [第一圈]
│   ├─ Navigation_Start_Record()  状态: RECORD
│   │
│   ├─ 每5ms: Navigation_Update(delta)
│   │   └─ 每5cm: nag_record_sample()
│   │       └─ 每500点: nag_flush_page() → 写Flash
│   │
│   └─ Navigation_Stop_Record()   剩余点写Flash + 写元数据
│                                 状态: IDLE
│
└─ [第二圈]
    ├─ Navigation_Start_Replay()  读元数据 + 预加载第一页
    │                             状态: REPLAY
    │
    ├─ 每5ms: Navigation_Update(delta)
    │   └─ 每5cm: replay_index++
    │       └─ nag_replay_get_yaw(index)
    │           └─ (按需) nag_load_page() ← 每25m换页读Flash
    │
    ├─ 每20ms: yaw_control(Navigation_Get_Yaw_Output())
    │
    └─ replay_index >= total_points
        └─ end_flag = 1, 状态: IDLE
```

---

## 七、关键参数说明

| 参数 | 值 | 含义 | 如何调整 |
|------|----|------|---------|
| `NAG_SAMPLE_DIST` | 0.05m | 采样间距 | 减小→更密更精准，但占用更多Flash；增大→省Flash但转弯处精度降低 |
| `NAG_YAW_SCALE` | 100 | 角度精度 | 100 对应 0.01° 精度，通常不需要改 |
| `NAG_POINTS_PER_PAGE` | 500 | 每页点数 | 受 Flash 页大小限制，不要随意改 |
| `NAG_DATA_START_PAGE` | 13 | 数据起始页 | 与 PID Flash（第11页）不重叠即可 |
| `NAG_DATA_END_PAGE` | 30 | 数据终止页 | 总容量 = (30-13+1)×500×5cm = 450m |

---

## 八、常见问题

**Q: 回放时车辆路线和第一圈有偏差，怎么办？**

偏差来源有三：
1. 陀螺仪漂移 — 确保 `Gyro_Offset_Init()` 校准充分（5秒静止）
2. 轮子半径误差 — `encode.c` 中 `WHEEL_RADUS` 已设为实测值 0.032m
3. 速度差异 — 两圈速度不同时，由于时间不同，IMU 积分误差不同。尽量保持两圈速度一致

**Q: 怎么知道记录了多少点？**

```c
uint16 n = Navigation_Get_Total_Points();
// n × 0.05m 就是记录的总路程
printf("记录了 %d 点, 约 %.1fm\n", n, n * 0.05f);
```

**Q: 第一圈数据怎么清除重录？**

```c
Navigation_Erase_All();   // 擦除所有导航Flash页
Navigation_Start_Record();  // 重新开始
```

**Q: `Navigation_Update` 在中断里安全吗？**

大部分情况安全。`nag_flush_page()` 写 Flash 耗时约 1~2ms，但只在每 25m 时触发一次。如果担心影响控制节拍，可以把写 Flash 操作移到主循环（加一个脏标志），中断里只做缓冲写入。

**Q: 最大能记录多少路程？**

`18页 × 500点 × 5cm = 450米`

标准比赛场地一圈通常在 30~60m，足够用。如果需要更长，减小 `NAG_SAMPLE_DIST`（但会提前填满）或增大 `NAG_DATA_END_PAGE`（确保不与其他 Flash 用途冲突）。
