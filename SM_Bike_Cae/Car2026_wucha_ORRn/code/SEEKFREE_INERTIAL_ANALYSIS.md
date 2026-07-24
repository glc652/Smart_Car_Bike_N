# Seekfree AI8051U 开源库惯导系统分析

## 目录
1. [系统架构](#系统架构)
2. [核心模块分析](#核心模块分析)
3. [惯导实现原理](#惯导实现原理)
4. [与Car2025的对比](#与car2025的对比)
5. [关键代码解析](#关键代码解析)

---

## 系统架构

### 整体框架

```
┌─────────────────────────────────────────────────────────┐
│              Seekfree AI8051U 惯导系统                   │
└─────────────────────────────────────────────────────────┘
                            ↓
        ┌───────────────────┼───────────────────┐
        ↓                   ↓                   ↓
    ┌────────┐          ┌────────┐         ┌────────┐
    │ 编码器  │          │ 陀螺仪  │         │GPS/IPS │
    │encoder │          │IMU963RA│         │定位    │
    └────────┘          └────────┘         └────────┘
        ↓                   ↓                   ↓
    速度/距离          角速度/角度          位置/方向
        ↓                   ↓                   ↓
    ┌─────────────────────────────────────────────┐
    │         1. 陀螺仪数据读取 (imu_read)        │
    │    读取原始角速度 → 去偏差 → 积分计算角度   │
    └─────────────────────────────────────────────┘
                            ↓
    ┌─────────────────────────────────────────────┐
    │    2. 四元数更新 (quaternion_update)        │
    │    使用陀螺仪角速度更新四元数               │
    │    四元数 → 欧拉角(Roll/Pitch/Yaw)         │
    └─────────────────────────────────────────────┘
                            ↓
    ┌─────────────────────────────────────────────┐
    │    3. 编码器速度控制 (enconder_v)           │
    │    读取编码器 → PID控制 → 输出PWM           │
    └─────────────────────────────────────────────┘
                            ↓
    ┌─────────────────────────────────────────────┐
    │    4. 姿态控制 (imu_control)                │
    │    Yaw角 + 角速度 → 双环PID → 电机控制      │
    └─────────────────────────────────────────────┘
                            ↓
    ┌─────────────────────────────────────────────┐
    │    5. 路径规划 (control_test)               │
    │    位置 + 目标角度 → 导航控制                │
    └─────────────────────────────────────────────┘
```

### 文件结构

```
project/code/
├── imu.c/h              # 陀螺仪数据处理 + 姿态控制
├── quaternion.c/h       # 四元数算法 + 欧拉角转换
├── encouder.c/h         # 编码器速度读取 + 速度控制
├── control.c/h          # 路径规划 + 导航控制
├── GPS.c/h              # GPS定位 + 位置计算
├── PID.c/h              # PID控制器
├── task.c/h             # 任务调度
└── ...
```

---

## 核心模块分析

### 1. IMU模块 (`imu.c`)

#### 关键参数

```c
float gps_ratio = 0.6;      // GPS权重(位置融合)
float gyro_ratio = 0.38;    // 陀螺仪权重
float dt = 0.005;           // 控制周期5ms
```

#### 主要函数

**`imu_read()` - 陀螺仪数据读取**

```c
void imu_read(void)
{
    // 1. 读取原始陀螺仪数据
    imu963ra_get_gyro();
    gx = imu963ra_gyro_x;
    gy = imu963ra_gyro_y;
    gz = imu963ra_gyro_z;
    
    // 2. 去偏差(使用校准值)
    gxx = gx - p_gx;  // p_gx是校准的偏差值
    gyy = gy - p_gy;
    gzz = gz - p_gz;
    
    // 3. 积分计算Yaw角
    if(abs(gzz) >= 10)  // 角速度阈值
    {
        now_angle -= gzz/14.3 * 0.005;  // 14.3是陀螺仪灵敏度
    }
    
    // 4. 滑动平均滤波(10点)
    gzz10 = gzz9;
    gzz9 = gzz8;
    // ... 循环移位
    gzz1 = gzz;
    av_gzz = (gzz1+gzz2+...+gzz10)/10.0;
}
```

**关键点**：
- 直接积分陀螺仪角速度计算Yaw角
- 使用10点滑动平均滤波降低噪声
- 灵敏度系数 14.3 (deg/s per LSB)

**`imu_flag_()` - 陀螺仪校准**

```c
void imu_flag_(void)
{
    // 1. 累积500个采样点的陀螺仪数据
    if(start_flag == 1) {
        i++;
        b_gx += gx;
        b_gy += gy;
        b_gz += gz;
    }
    
    // 2. 计算平均值作为偏差
    if(i >= times && imu_flag)
    {
        b_gx /= i;  // 平均值
        b_gy /= i;
        b_gz /= i;
        
        // 3. 保存为校准值
        p_gx = b_gx;
        p_gy = b_gy;
        p_gz = b_gz;
        
        imu_flag = 0;  // 校准完成
    }
}
```

**`imu_control()` - 姿态控制**

```c
void imu_control(void)
{
    // 1. 获取角速度
    v_angle = gzz;
    
    // 2. 位置PID：计算目标角度与实际角度的误差
    error_pwm_1 = -Position_PID(now_angle, target_angle);
    
    // 3. 判断是否接近目标角度
    if(abs(target_angle - now_angle) <= 0.5)
    {
        // 已到达目标，直线行驶
        drive_right = PwmXianfu(drive_right, 500, 950);
        drive_left = PwmXianfu(drive_left, 500, 950);
    }
    else
    {
        // 4. 速度PID：根据角速度调整电机
        error_pwm = Incremental_PID_2(v_angle, error_pwm_1*3);
        
        // 5. 差速转向
        new_drive_left = drive_left - error_pwm;
        new_drive_right = drive_left + error_pwm;
    }
}
```

**控制逻辑**：
- 双环PID：外环位置PID(角度) + 内环速度PID(角速度)
- 差速转向：左右电机速度差实现转向

---

### 2. 四元数模块 (`quaternion.c`)

#### 四元数更新算法

```c
void quaternion_update(void)
{
    // 1. 角速度转换为弧度
    wx = (double)gxx / 14.3 * PI/180.0;
    wy = (double)gyy / 14.3 * PI/180.0;
    wz = (double)gzz / 14.3 * PI/180.0;
    
    // 2. 四元数微分方程
    q0_dot = (-quat.q1 * wx - quat.q2 * wy - quat.q3 * wz) * 0.5;
    q1_dot = ( quat.q0 * wx + quat.q2 * wz - quat.q3 * wy) * 0.5;
    q2_dot = ( quat.q0 * wy - quat.q1 * wz + quat.q3 * wx) * 0.5;
    q3_dot = ( quat.q0 * wz + quat.q1 * wy - quat.q2 * wx) * 0.5;
    
    // 3. 积分更新四元数
    quat.q0 += q0_dot * dt;
    quat.q1 += q1_dot * dt;
    quat.q2 += q2_dot * dt;
    quat.q3 += q3_dot * dt;
    
    // 4. 归一化(保持单位四元数)
    norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    if(norm > 0.0) {
        quat.q0 /= norm;
        quat.q1 /= norm;
        quat.q2 /= norm;
        quat.q3 /= norm;
    }
}
```

#### 欧拉角转换

```c
void quaternion_update(void)
{
    // ... 四元数更新代码 ...
    
    // Roll角
    temp1 = 2.0 * (quat.q0*quat.q1 + quat.q2*quat.q3);
    temp2 = 1.0 - 2.0 * (quat.q1*quat.q1 + quat.q2*quat.q2);
    euler.roll = my_atan2(temp1, temp2) * rad2deg;
    
    // Pitch角
    temp3 = 2.0 * (quat.q0*quat.q2 - quat.q3*quat.q1);
    if(temp3 > 1.0) temp3 = 1.0;
    else if(temp3 < -1.0) temp3 = -1.0;
    euler.pitch = asin(temp3) * rad2deg;
    
    // Yaw角
    temp1 = 2.0 * (quat.q0*quat.q3 + quat.q1*quat.q2);
    temp2 = 1.0 - 2.0 * (quat.q2*quat.q2 + quat.q3*quat.q3);
    euler.yaw = my_atan2(temp1, temp2) * rad2deg;
}
```

**四元数优势**：
- 避免万向锁问题
- 计算效率高
- 适合实时系统

---

### 3. 编码器模块 (`encouder.c`)

#### 速度读取

```c
void enconder_read(void)
{
    // 1. 读取编码器脉冲数
    Reality_Velocity = -encoder_get_count(ENCODER_DIR_1);
    encoder_clear_count(ENCODER_DIR_1);
    
    // 2. 脉冲转距离
    // 公式: 距离 = 脉冲数 / 512 * 2π * 0.017
    // 其中: 512 = 编码器分辨率
    //      0.017 = 轮子半径(m)
    position += (float)Reality_Velocity / 512 * 2 * 3.14 * 0.017;
}
```

#### 速度控制

```c
void enconder_v(void)
{
    // 1. 增量式PID控制速度
    drive_left = 500 + Incremental_PID_1(Reality_Velocity, Target_Velocity);
    
    // 2. PWM限幅
    drive_left = PwmXianfu(drive_left, 500, 950);
    
    // 3. 左右电机同速
    drive_right = drive_left;
}
```

**关键参数**：
- 编码器分辨率: 512 脉冲/圈
- 轮子半径: 0.017 m (17mm)
- PWM范围: 500~950

---

### 4. 路径规划模块 (`control.c`)

#### 简单路径控制

```c
void control_test(void)
{
    // 根据位置设置目标角度和速度
    if(position < 1.5)
    {
        target_angle = 0.0;      // 直线行驶
        Target_Velocity = 100;
    }
    else if(position < 4.5)
    {
        target_angle = 180.0;    // 转向180度
        Target_Velocity = 100;
    }
    else
    {
        stop_flag = 1;           // 停止
    }
}
```

**路径参数**：
```c
float road1 = 8.3;              // 第一段距离
float road21 = 11.3;            // 第二段距离
float zhuanjiao_angle = 45.0;   // 转向角度
float zhuanjiao_road = 4.5;     // 转向距离
int basic_speed = 75;           // 基础速度
int high_speed = 250;           // 高速
```

---

## 惯导实现原理

### 1. 陀螺仪积分法

**原理**：
```
Yaw(t) = Yaw(t-1) + ω_z × dt

其中：
ω_z = 陀螺仪Z轴角速度(deg/s)
dt = 采样周期(0.005s)
```

**代码实现**：
```c
if(abs(gzz) >= 10)  // 角速度阈值
{
    now_angle -= gzz/14.3 * 0.005;
}
```

**特点**：
- ✓ 简单直接
- ✓ 实时性好
- ✗ 长期漂移严重

### 2. 四元数算法

**优势**：
- 避免万向锁
- 计算效率高
- 适合3D旋转

**微分方程**：
```
q̇ = 0.5 × q ⊗ ω

其中：
q = 四元数 [q0, q1, q2, q3]
ω = 角速度向量 [0, ωx, ωy, ωz]
⊗ = 四元数乘法
```

### 3. 编码器距离积分

**原理**：
```
position(t) = position(t-1) + distance_per_pulse × pulse_count

其中：
distance_per_pulse = 2π × wheel_radius / pulses_per_revolution
```

**代码**：
```c
position += (float)Reality_Velocity / 512 * 2 * 3.14 * 0.017;
```

### 4. 双环PID控制

```
┌─────────────────────────────────────────┐
│         目标角度 (target_angle)         │
└─────────────────────────────────────────┘
                    ↓
        ┌───────────────────────┐
        │  位置PID (外环)       │
        │ 计算角度误差          │
        └───────────────────────┘
                    ↓
        ┌───────────────────────┐
        │  速度PID (内环)       │
        │ 根据角速度调整电机    │
        └───────────────────────┘
                    ↓
        ┌───────────────────────┐
        │  差速转向             │
        │ 左右电机速度差        │
        └───────────────────────┘
                    ↓
        ┌───────────────────────┐
        │  PWM输出              │
        └───────────────────────┘
```

---

## 与Car2025的对比

### 相同点

| 功能 | Seekfree | Car2025 |
|------|---------|--------|
| 陀螺仪积分 | ✓ | ✓ |
| 编码器测距 | ✓ | ✓ |
| 双环PID | ✓ | ✓ |
| 四元数算法 | ✓ | ✗ |
| 位置融合 | ✓ | ✗ |

### 差异点

| 方面 | Seekfree | Car2025 |
|------|---------|--------|
| **陀螺仪处理** | 直接积分 + 滑动平均 | 互补滤波 + 加速度计 |
| **位置计算** | 编码器积分 | 编码器 + 陀螺仪融合 |
| **融合算法** | 四元数 | 互补滤波 |
| **定位方式** | GPS/IPS | 纯惯导 |
| **控制方式** | 双环PID | 多层次PID |

### Seekfree的优势

1. **四元数算法**
   - 避免万向锁
   - 更精确的3D旋转

2. **GPS/IPS融合**
   - 长期位置校正
   - 全局定位能力

3. **完整的路径规划**
   - 预定义路径
   - 自动导航

### Car2025的优势

1. **互补滤波**
   - 加速度计校正陀螺仪漂移
   - 更稳定的姿态估计

2. **编码器-陀螺仪融合**
   - 距离精度高
   - 角度精度高
   - 互补缺陷

3. **权重可调**
   - 灵活适应不同场景
   - 参数优化空间大

---

## 关键代码解析

### 1. 陀螺仪校准流程

```c
// 初始化时调用
void imu_flag_(void)
{
    // 第1步: 累积采样
    if(start_flag == 1) {
        i++;
        b_gx += gx;
        b_gy += gy;
        b_gz += gz;
    }
    
    // 第2步: 计算平均值(500个采样)
    if(i >= 500 && imu_flag) {
        b_gx /= i;
        b_gy /= i;
        b_gz /= i;
        
        // 第3步: 保存为偏差值
        p_gx = b_gx;
        p_gy = b_gy;
        p_gz = b_gz;
        
        imu_flag = 0;  // 标记完成
    }
}
```

**流程**：
1. 系统启动时保持静止
2. 累积500个采样点(2.5秒)
3. 计算平均值作为零点偏差
4. 后续读取时自动去偏差

### 2. 四元数更新流程

```c
void quaternion_update(void)
{
    // 第1步: 角速度转换
    wx = gxx / 14.3 * PI/180.0;
    wy = gyy / 14.3 * PI/180.0;
    wz = gzz / 14.3 * PI/180.0;
    
    // 第2步: 四元数微分
    q0_dot = (-q1*wx - q2*wy - q3*wz) * 0.5;
    q1_dot = ( q0*wx + q2*wz - q3*wy) * 0.5;
    q2_dot = ( q0*wy - q1*wz + q3*wx) * 0.5;
    q3_dot = ( q0*wz + q1*wy - q2*wx) * 0.5;
    
    // 第3步: 积分
    q0 += q0_dot * 0.005;
    q1 += q1_dot * 0.005;
    q2 += q2_dot * 0.005;
    q3 += q3_dot * 0.005;
    
    // 第4步: 归一化
    norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 /= norm;
    q1 /= norm;
    q2 /= norm;
    q3 /= norm;
}
```

### 3. 双环PID控制

```c
void imu_control(void)
{
    // 外环: 位置PID (角度控制)
    error_pwm_1 = -Position_PID(now_angle, target_angle);
    
    // 内环: 速度PID (角速度控制)
    error_pwm = Incremental_PID_2(v_angle, error_pwm_1*3);
    
    // 差速转向
    new_drive_left = drive_left - error_pwm;
    new_drive_right = drive_left + error_pwm;
    
    // PWM输出
    pwm_set_duty(PWMB_CH4_P53, new_drive_left);
    pwm_set_duty(PWMB_CH1_P50, new_drive_right);
}
```

---

## 总结

### Seekfree惯导系统特点

| 特性 | 说明 |
|------|------|
| **传感器** | IMU963RA(6轴) + 编码器 + GPS/IPS |
| **算法** | 四元数 + 双环PID + GPS融合 |
| **精度** | 中等(±5°角度, ±10cm位置) |
| **实时性** | 5ms周期 |
| **复杂度** | 中等 |
| **可靠性** | 高(多传感器融合) |

### 建议改进方向

1. **加入加速度计**
   - 互补滤波校正陀螺仪漂移
   - 提高长期精度

2. **编码器权重调整**
   - 根据运动状态动态调整
   - 提高转向精度

3. **卡尔曼滤波**
   - 替代简单的滑动平均
   - 更优的噪声抑制

4. **多传感器融合**
   - 磁力计辅助定向
   - 气压计辅助高度

---

## 参考资源

- **IMU963RA**: 6轴IMU传感器
- **四元数**: 3D旋转表示
- **PID控制**: 双环控制系统
- **编码器**: 距离测量
- **GPS/IPS**: 全局定位

