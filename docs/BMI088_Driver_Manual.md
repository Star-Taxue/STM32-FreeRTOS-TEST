# BMI088 驱动开发手册

> Bosch BMI088 6轴IMU (SPI) | STM32F407VET6 | FreeRTOS

---

## 1. 传感器关键参数

| 参数 | 加速度计 | 陀螺仪 |
|------|----------|--------|
| **芯片ID** | `0x1E` | `0x0F` |
| **量程** | ±6g | ±2000°/s |
| **灵敏度** | 5460 LSB/g | 16.384 LSB/°/s |
| **ODR** | 100Hz | 1000Hz |

### 寄存器速查

```
加速度计:  CHIP_ID=0x00  DATA=0x12~0x17  CONF=0x40  RANGE=0x41  PWR_CONF=0x7C  PWR_CTRL=0x7D  SOFTRESET=0x7E
陀螺仪:    CHIP_ID=0x00  DATA=0x02~0x07  RANGE=0x0F  BANDWIDTH=0x10  SOFTRESET=0x14
```

---

## 2. 硬件接口

### 2.1 引脚连接

```
SPI1:  PA5(SCK)  PA6(MISO)  PA7(MOSI)
片选:  PB0(ACC_CS)  PB1(GYRO_CS)    ← GPIO_Output, 默认高电平
```

### 2.2 SPI配置

```c
Mode: SPI_MODE_MASTER    |  DataSize: 8BIT    |  NSS: SOFT
CPOL: LOW (0)            |  CPHA: 1EDGE (0)   |  FirstBit: MSB
Prescaler: 32            |  Direction: 2LINES
```

### 2.3 双片选设计

BMI088 内部加速度计和陀螺仪是**两个独立芯片**封装在一起，共享 SPI 总线但片选独立。操作时必须分别拉低对应 CS。

---

## 3. 数据处理 (⚠️ 易错点)

### 3.1 SPI 读写时序

```c
// 读: 发送 {reg | 0x80, 0x00}，接收2字节，有效数据在 rx_data[1]
// 写: 发送 {reg & 0x7F, data}
```

### 3.2 连续读取时的数据排列

从 `0x12` (ACC_X_LSB) 开始连续读7字节，SPI 返回:

```
rx_data[0] = 哑字节 (读命令的回应)
rx_data[1] = ACC_X_LSB
rx_data[2] = ACC_X_MSB
rx_data[3] = ACC_Y_LSB
rx_data[4] = ACC_Y_MSB
rx_data[5] = ACC_Z_LSB
rx_data[6] = ACC_Z_MSB
```

### 3.3 大小端拼接

**加速度计**: LSB 在前, MSB 在后 → `MSB << 8 | LSB`
```c
raw_x = (rx_data[1] << 8) | rx_data[2];  // rx_data[1]=LSB, rx_data[2]=MSB
raw_y = (rx_data[3] << 8) | rx_data[4];
raw_z = (rx_data[5] << 8) | rx_data[6];
```

**陀螺仪**: 同样 LSB 在前
```c
raw_x = (rx_data[2] << 8) | rx_data[1];  // rx_data[1]=LSB, rx_data[2]=MSB
raw_y = (rx_data[4] << 8) | rx_data[3];
raw_z = (rx_data[6] << 8) | rx_data[5];
```

> ⚠️ **注意**: 加速度计和陀螺仪的 rx_data 索引不同！因为加速度计从 0x12 开始读，陀螺仪从 0x02 开始读，SPI 返回的字节偏移不同。

### 3.4 轴映射 (PCB安装方向)

```c
// 根据PCB物理安装方向做轴旋转
accel[0] = raw_y / 5460.0f;   // X轴 ← 传感器Y
accel[1] = raw_z / 5460.0f;   // Y轴 ← 传感器Z
accel[2] = raw_x / 5460.0f;   // Z轴 ← 传感器X

gyro[0] = raw_x / 16.384f;    // 陀螺仪不做轴映射
gyro[1] = raw_y / 16.384f;
gyro[2] = raw_z / 16.384f;
```

> ⚠️ 轴映射取决于 PCB 上传感器的安装方向。如果平放时 Roll/Pitch 不为 0，首先检查轴映射是否正确。

---

## 4. 初始化流程

### 4.1 加速度计 (关键步骤)

```
1. CS 拉高 → 延时 → CS 拉低 → SPI读命令 → CS 拉高    ← SPI模式激活 (必须!)
2. 写 SOFTRESET = 0xB6 → 延时 50ms
3. 读 CHIP_ID (0x00)，期望 0x1E，失败则重试
4. 写 PWR_CONF (0x7C) = 0x00    ← 退出节能模式
5. 写 ACC_CONF (0x40) = 0xA8    ← ODR=100Hz
6. 写 ACC_RANGE (0x41) = 0x01   ← ±6g
7. 写 PWR_CTRL (0x7D) = 0x04    ← 使能传感器 (关键!)
8. 延时 50ms 等待稳定
```

### 4.2 陀螺仪

```
1. 延时 1000ms (上电等待)
2. 读 CHIP_ID (0x00)，期望 0x0F
3. 写 GYRO_RANGE (0x0F) = 0x00    ← ±2000°/s
4. 写 GYRO_BANDWIDTH (0x10) = 0x02 ← ODR=1000Hz
```

### 4.3 片选引脚初始状态

```c
// ⚠️ 必须在 MX_GPIO_Init() 中初始化为高电平!
HAL_GPIO_WritePin(GPIOB, ACC_CS_Pin|GYRO_CS_Pin, GPIO_PIN_SET);
```

> ⚠️ 如果初始化为低电平，BMI088 上电时可能进入错误状态，导致读取到错误的芯片ID（如读到陀螺仪ID 0x0F 而非加速度计ID 0x1E）。

---

## 5. 姿态解算 (互补滤波)

### 5.1 算法

```
角度 = α × 陀螺仪积分 + (1-α) × 加速度计角度    (α = 0.98)
```

### 5.2 加速度计求角度

```c
roll  = atan2(accel[1], accel[2]) × 180/π
pitch = atan2(-accel[0], √(accel[1]² + accel[2]²)) × 180/π
```

### 5.3 数据有效性检查

```c
// 加速度模长应接近 1g (静止状态)
if (|√(x²+y²+z²) - 1.0| < 0.3) {
    α = 0.98;  // 正常融合
} else {
    α = 0.99;  // 运动中，主要依赖陀螺仪
}
```

### 5.4 YAW 零偏校准 (减缓漂移)

陀螺仪静止时也有微小输出（零偏），积分后持续累积导致 YAW 漂移。
**方案：上电时设备静止，采集 N 次陀螺仪数据求平均作为零偏，后续读数减去零偏。**

```c
// 上电后调用（设备必须静止！）
float gyro_raw[3];
for (int i = 0; i < 200; i++) {
    BSP_BMI088_ReadGyro(gyro_raw);
    HAL_Delay(2);
}
BSP_IMU_CalibrateBias(gyro_raw, 200);  // 校准零偏
```

校准后积分公式变为：
```
yaw += (gyro_z - gyro_bias_z) × dt
```

> ⚠️ 零偏校准只能**减缓**漂移，不能完全消除。长时间运行仍有累积误差。彻底解决需磁力计。

---

## 6. API 参考

```c
// BSP层 - BMI088驱动
BSP_BMI088_Status_t BSP_BMI088_Init(void);
void BSP_BMI088_ReadAccel(float accel[3]);   // 单位: g
void BSP_BMI088_ReadGyro(float gyro[3]);     // 单位: °/s

// BSP层 - 姿态解算
void BSP_IMU_Init(void);
void BSP_IMU_CalibrateBias(float *gyro_raw, uint16_t samples);  // 上电零偏校准
void BSP_IMU_Update(float *accel, float *gyro, float dt);
void BSP_IMU_GetAngle(euler_angle_t *angle);
```

---

## 7. 常见问题排查

| 现象 | 原因 | 解决 |
|------|------|------|
| ACC ID 读到 0x0F | 片选初始为低 / SPI未激活 | CS 初始拉高，执行SPI激活流程 |
| 数据全为0 | 传感器未使能 | 检查 PWR_CTRL=0x04 |
| 平放 Roll/Pitch ≠ 0 | 轴映射错误 | 检查 accel[0/1/2] 映射关系 |
| 数据正负颠倒 | 大小端拼接错误 | 检查 MSB/LSB 顺序 |
| 数据跳变 | SPI时序/延时不足 | 增加 CS 切换后的 NOP 延时 |
| YAW 持续漂移 | 陀螺仪零偏未校准 | 上电调用 BSP_IMU_CalibrateBias() |

---

## 8. 项目配置参考

| 配置项 | 值 |
|--------|-----|
| 芯片 | STM32F407VET6 |
| RTOS | FreeRTOS (CMSIS-OS2) |
| 构建 | CMake |
| SPI | SPI1, 32分频, Mode0 |
| 架构 | APP → BSP → DRV → HAL |
| 文件 | `Core/Bsp/` (传感器驱动), `Core/Drv/` (SPI封装) |
