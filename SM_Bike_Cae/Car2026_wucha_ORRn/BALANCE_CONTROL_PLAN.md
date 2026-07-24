# 舵机单车自平衡控制方案

## 1. 系统架构

### 1.1 多频率分层控制

```
┌─────────────────────────────────────────────────────────────┐
│                    主控制循环 (1ms)                          │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 1. 读取IMU数据 (陀螺仪 + 加速度计)                   │  │
│  │ 2. 计算滚动角/俯仰角/偏航角                          │  │
│  │ 3. 执行多频率分层控制                                │  │
│  └──────────────────────────────────────────────────────┘  │
│                          │                                   │
│         ┌────────────────┼────────────────┐                 │
│         ▼                ▼                ▼                 │
│    ┌─────────┐      ┌─────────┐      ┌─────────┐           │
│    │ 1ms     │      │ 5ms     │      │ 20ms    │           │
│    │ 角速度  │      │ 角度    │      │ 航向    │           │
│    │ 环      │      │ 环      │      │ 环      │           │
│    └────┬────┘      └────┬────┘      └────┬────┘           │
│         │                │                │                 │
│         ▼                ▼                ▼                 │
│    舵机控制        更新角速度目标    航向控制               │
│    (转向)         (平衡)            (导航)                 │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 级联PID结构

```
目标滚动角 (0°)
    │
    ▼
┌─────────────────────────┐
│  角度环 (5ms, PD)       │  Kp=4.5, Kd=0
│  输入: Roll_Angle       │
│  输出: 目标角速度       │
└────────┬────────────────┘
         │ angle_put
         ▼
┌─────────────────────────────────────────┐
│  角速度环 (1ms, PD)                     │
│  Kp = f(Encoder_Speed)                  │
│  Kd = 0.4                               │
│  输入: IMU_Gyro_y (低通滤波)            │
│  输出: 舵机转向命令                     │
└────────┬────────────────────────────────┘
         │
         ▼
    舵机执行
```

## 2. 控制算法详解

### 2.1 角速度环 (1ms, 最高优先级)

**特点:**
- 频率最高 (1ms)，响应最快
- 必须进行低通滤波，防止抖动
- 仅有角速度环时，滚动角会缓慢改变，舵机略微变动

**低通滤波实现:**
```c
// 一阶低通滤波
gyro_y_filtered = gyro_y_filtered * 0.8 + gyro_y_raw * 0.2;
```

**PID参数:**
- Kp = f(Encoder_Speed)  // 目标角速度与实际小车速度的关系
- Ki = 0
- Kd = 0.4

**速度自适应关系:**

目标角速度应该与小车实际速度成正比关系。速度越快，需要的转向角速度越大才能完成转向。

```c
// 例如：
// 当小车速度为0时，Kp应该较小（低速时转向灵敏度低）
// 当小车速度增加时，Kp应该增加（高速时需要更大的转向角速度）

float speed_adaptive_kp = base_kp + speed_factor * Encoder_Speed;
// 或者用分段函数根据实际情况调整
```

**控制逻辑:**
```
输入: angle_put (来自5ms角度环)
处理:
  1. 低通滤波陀螺仪数据
  2. 计算误差: error = angle_put - gyro_y_filtered
  3. PD控制: output = Kp(speed) * error + Kd * d_error
  4. 限幅: [-300, 300]
输出: Servo_Set(servo_output)
```

### 2.2 角度环 (5ms, 中等优先级)

**特点:**
- 角度环 + 角速度环级联才能保持平衡
- 单独角度环无法平衡 (需要角速度反馈)
- 输出作为角速度环的目标值

**PID参数:**
- Kp = 4.5
- Ki = 0
- Kd = 0

**控制逻辑:**
```
输入: Roll_Angle (从IMU读取)
处理:
  1. 计算误差: error = 0 - Roll_Angle
  2. P控制: angle_put = Kp * error
  3. 限幅: [-300, 300]
输出: angle_put (作为角速度环目标)
```

### 2.3 航向环 (20ms, 低优先级)

**特点:**
- 舵机往左旋转 → 车子向右转
- 不能直接用 (目标 - 当前) 作为控制量
- 采用转向误差积累方式，平滑趋近目标转向

**PID参数:**
- Kp = 0.9
- Ki = 0
- Kd = 0

**控制逻辑:**
```
输入: YawAngle_Trans (当前偏航角), yaw_target (目标偏航角)
处理:
  1. 计算误差: error = yaw_target - YawAngle_Trans
  2. 处理±180°跳变
  3. 误差积累: yaw_put += Kp * error  (关键：是累加，不是直接赋值)
  4. 限幅: [-300, 300]
输出: yaw_put (平滑趋近的转向命令)
```

**为什么用误差积累:**
- 直接用 `yaw_put = Kp * error` 会导致舵机突然大幅转向
- 用 `yaw_put += Kp * error` 使转向命令逐步累积，实现平滑趋近
- 每个周期只增加一小部分，避免摇摆

## 3. 速度自适应转向

### 3.1 问题分析

**开环现象:** 单车跑了几米后左右摇摆
- **原因:** 速度改变但舵机转向没变
- **解决:** 根据速度动态调整角速度环的Kp

### 3.2 实现方式

**关键理解:** 目标角速度与实际小车速度的关系

```c
// 角速度环的Kp应该随小车速度变化
// 速度快时，需要更大的Kp来产生足够的转向角速度
// 速度慢时，Kp可以较小

float speed_adaptive_kp = base_kp + speed_factor * Encoder_Speed;

// 例如:
// base_kp = 1.0 (基础值)
// speed_factor = 0.05 (速度因子)
// 速度 = 0   → Kp = 1.0
// 速度 = 10  → Kp = 1.5
// 速度 = 20  → Kp = 2.0
```

**效果:**
- 速度快时，Kp增大，舵机转向增大，产生足够的角加速度
- 速度慢时，Kp减小，舵机转向减小，防止过度转向
- 自动适应不同速度下的平衡需求

## 4. 科目二特殊处理

### 4.1 科目二 (八字慢速) 控制策略

**不使用角速度环，仅用:**
- 角度环 (5ms)
- 方向环 (20ms)

**原因:**
- 低速运动，不需要快速角速度反馈
- 角度环直接控制舵机转向
- 方向环用于路径跟踪

### 4.2 遥控器集成

**方向环目标值来源:**
- 遥控器输入 (推荐)
- 或自动路径规划

**实现:**
```c
// 遥控器值作为方向环目标
yaw_target = remote_control_value;  // [-300, 300]
// 误差积累方式，平滑趋近
float yaw_error = yaw_target - YawAngle_Trans;
if(yaw_error > 180) yaw_error -= 360;
if(yaw_error < -180) yaw_error += 360;
yaw_put += Kp * yaw_error;  // 积累，不是直接赋值
```

## 5. 实现细节

### 5.1 定时器配置 (cpu0_main.c)

```c
void PIT_Init(void){
    pit_ms_init(CCU61_CH0, 1);   // 1ms - 角速度环
    pit_ms_init(CCU61_CH1, 5);   // 5ms - 角度环
}
```

### 5.2 中断处理 (isr.c)

#### 1ms 中断 - 角速度环 (CCU6_1_CH0)

```c
IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);
    pit_clear_flag(CCU61_CH0);

    // 低通滤波
    static float gyro_y_filtered = 0;
    gyro_y_filtered = gyro_y_filtered * 0.8 + IMU_Data.Gyro_y * 0.2;

    // 速度自适应Kp：目标角速度与实际小车速度的关系
    float speed_adaptive_kp = 1.0 + 0.05 * Encoder_Speed;

    // 角速度PD控制
    static float last_error = 0;
    float error = angle_put - gyro_y_filtered;
    float servo_output = speed_adaptive_kp * error + 0.4 * (error - last_error);
    last_error = error;

    // 限幅
    if(servo_output > 300) servo_output = 300;
    if(servo_output < -300) servo_output = -300;

    Servo_Set(servo_output);
}
```

#### 5ms 中断 - 角度环 (CCU6_1_CH1)

```c
IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);
    pit_clear_flag(CCU61_CH1);

    // 角度PD控制
    float error = 0 - Roll_Angle;
    angle_put = 4.5 * error;

    // 限幅
    if(angle_put > 300) angle_put = 300;
    if(angle_put < -300) angle_put = -300;

    // 速度控制
    Set_Car_Speed(20);
}
```

#### 20ms 航向环 - 在主循环中处理

```c
// 在 Ctrl_main() 或其他地方处理
static uint8 cnt_20ms = 0;
if(++cnt_20ms >= 20){
    cnt_20ms = 0;

    // 航向误差积累
    float yaw_error = yaw_target - YawAngle_Trans;

    // 处理±180°跳变
    if(yaw_error > 180) yaw_error -= 360;
    if(yaw_error < -180) yaw_error += 360;

    // P控制，误差积累
    yaw_put += 0.9 * yaw_error;

    // 限幅
    if(yaw_put > 300) yaw_put = 300;
    if(yaw_put < -300) yaw_put = -300;
}
```

### 5.3 主控制循环 (Car_control.c)

```c
void Ctrl_main(void){
    static uint8 cnt_20ms = 0;

    if (!protection){
        Gyro_Get_All_Angles();
        Get_Angle_Value(&Yaw_Angle, &Pitch_Angle, &Roll_Angle);
        Encoder_Process();
        car_safety();

        // 20ms 航向环处理
        if(++cnt_20ms >= 20){
            cnt_20ms = 0;

            float yaw_error = yaw_target - YawAngle_Trans;
            if(yaw_error > 180) yaw_error -= 360;
            if(yaw_error < -180) yaw_error += 360;

            yaw_put += 0.9 * yaw_error;
            if(yaw_put > 300) yaw_put = 300;
            if(yaw_put < -300) yaw_put = -300;
        }

        // 1ms和5ms的控制在中断中执行
    }else{
        Servo_Set(0);
        Motor_Disable();
    }
}
```

## 6. 参数调试流程

### 6.1 第一步：调试角速度环 (1ms)

1. **禁用角度环** (angle_put = 0)
2. **手动倾斜车子** 观察舵机响应
3. **调整低通滤波系数** (0.8/0.2)
   - 增大0.8：滤波更强，但响应延迟
   - 减小0.8：响应快，但可能抖动
4. **调整Kd** (0.4) 改善阻尼
5. **加入速度自适应** 调整 base_kp 和 speed_factor

**目标:** 有点抖但落地不抖

### 6.2 第二步：调试角度环 (5ms)

1. **启用角度环**
2. **观察平衡效果**
3. **调整Kp** (4.5)
   - 太小：平衡慢
   - 太大：振荡
4. **验证级联效果**

**目标:** 角度环 + 角速度环能保持平衡

### 6.3 第三步：调试航向环 (20ms)

1. **设置yaw_target**
2. **观察转向响应**
3. **调整Kp** (0.9)
4. **验证误差积累方式**

**目标:** 平滑趋近目标转向

### 6.4 第四步：速度自适应

1. **测试不同速度**
2. **调整base_kp** (1.0)
3. **调整speed_factor** (0.05)
4. **验证防摇摆效果**

**目标:** 各速度下都能平衡

## 7. 科目一 vs 科目二

### 科目一 (直线竞速)

| 环节 | 频率 | 类型 | 目标 |
|------|------|------|------|
| 角速度 | 1ms | PD | 快速转向响应 |
| 角度 | 5ms | PD | 保持平衡 |
| 航向 | 20ms | P | 直线行驶 |

### 科目二 (八字慢速)

| 环节 | 频率 | 类型 | 目标 |
|------|------|------|------|
| 角度 | 5ms | PD | 保持平衡 |
| 航向 | 20ms | P | 路径跟踪 |
| 遥控 | 实时 | 手动 | 方向控制 |

**关键差异:**
- 科目一：自动平衡 + 自动导航
- 科目二：自动平衡 + 遥控转向

## 8. 安全保护

```c
void car_safety(void){
    // 滚动角超过±50°立即停止
    if (Roll_Angle > 50.0f || Roll_Angle < -50.0f) {
        Motor_Disable();
        Servo_Set(0);
        protection = 1;
    }
}
```

## 9. 调试建议

1. **先调1ms角速度环** - 基础
   - 从禁用角度环开始
   - 手动倾斜观察舵机响应
   - 加入低通滤波消除抖动

2. **再加5ms角度环** - 平衡
   - 启用角度环
   - 观察自动平衡效果
   - 调整Kp使平衡稳定

3. **加入速度自适应** - 防摇摆
   - 测试不同速度
   - 调整base_kp和speed_factor
   - 验证各速度下都能平衡

4. **最后加20ms航向环** - 导航
   - 设置yaw_target
   - 验证误差积累方式
   - 调整Kp使转向平滑

5. **用遥控器微调** - 精细控制
   - 科目二中用遥控器控制方向
   - 实时调整参数

## 10. 常见问题

### Q: 为什么需要低通滤波?
**A:** 1ms采样频率高，陀螺仪噪声会导致舵机抖动。低通滤波平滑数据。

### Q: 为什么角速度环Kp要随速度变化?
**A:** 目标角速度与实际小车速度成正比。速度快时需要更大的Kp来产生足够的转向角速度，速度慢时Kp可以较小。

### Q: 为什么航向环用误差积累而不是直接差值?
**A:**
- **直接差值** `yaw_put = Kp * error` 会导致舵机突然大幅转向，容易摇摆
- **误差积累** `yaw_put += Kp * error` 使转向命令逐步累积，每个周期只增加一小部分，实现平滑趋近目标转向
- 这样可以避免转向过快导致的超调和摇摆

### Q: 科目二为什么不用角速度环?
**A:** 低速运动，角速度反馈不必要。角度环直接控制舵机更简单有效。

### Q: 只有两个中断，20ms航向环怎么处理?
**A:** 在主循环Ctrl_main()中用计数器实现20ms周期，或在其他现有的20ms中断中处理。

## 11. 性能指标

| 指标 | 目标 | 说明 |
|------|------|------|
| 平衡时间 | <500ms | 从倾斜到平衡 |
| 舵机响应 | <50ms | 1ms环的反应时间 |
| 转向平滑度 | 无突跳 | 误差积累方式 |
| 速度范围 | 0-30cm/s | 自适应Kp覆盖 |
| 最大倾角 | ±50° | 安全保护限制 |
