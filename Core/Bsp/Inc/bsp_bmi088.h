/**
 * @file    bsp_bmi088.h
 * @brief   BMI088传感器BSP层驱动
 * @author  STM32 Team
 * @date    2026-04-09
 * @version V1.0
 *
 * @description
 * BMI088 6轴传感器驱动头文件，封装硬件细节，提供简洁的数据读取接口。
 * 基于DRV层SPI接口实现，负责传感器初始化、配置和数据读取。
 */

#ifndef BSP_BMI088_H
#define BSP_BMI088_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Inc/drv_spi.h"
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/

/* BMI088 加速度计寄存器地址 */
#define BMI088_ACC_CHIP_ID          0x00
#define BMI088_ACC_CHIP_ID_VALUE    0x1E
#define BMI088_ACC_ERR_REG          0x02
#define BMI088_ACC_STATUS           0x03
#define BMI088_ACC_X_LSB            0x12
#define BMI088_ACC_X_MSB            0x13
#define BMI088_ACC_Y_LSB            0x14
#define BMI088_ACC_Y_MSB            0x15
#define BMI088_ACC_Z_LSB            0x16
#define BMI088_ACC_Z_MSB            0x17
#define BMI088_ACC_CONF             0x40
#define BMI088_ACC_RANGE            0x41
#define BMI088_ACC_PWR_CONF         0x7C
#define BMI088_ACC_PWR_CTRL         0x7D
#define BMI088_ACC_SOFTRESET        0x7E

/* BMI088 陀螺仪寄存器地址 */
#define BMI088_GYRO_CHIP_ID         0x00
#define BMI088_GYRO_CHIP_ID_VALUE   0x0F
#define BMI088_GYRO_RATE_X_LSB      0x02
#define BMI088_GYRO_RATE_X_MSB      0x03
#define BMI088_GYRO_RATE_Y_LSB      0x04
#define BMI088_GYRO_RATE_Y_MSB      0x05
#define BMI088_GYRO_RATE_Z_LSB      0x06
#define BMI088_GYRO_RATE_Z_MSB      0x07
#define BMI088_GYRO_RANGE           0x0F
#define BMI088_GYRO_BANDWIDTH       0x10
#define BMI088_GYRO_SOFTRESET       0x14

/* 传感器配置值 */
#define BMI088_ACC_RANGE_6G         0x01
#define BMI088_ACC_ODR_100HZ        0xA8
#define BMI088_GYRO_RANGE_2000DPS   0x00
#define BMI088_GYRO_ODR_1000HZ      0x02

/* 灵敏度系数 */
#define BMI088_ACC_SENSITIVITY      5460.0f
#define BMI088_GYRO_SENSITIVITY     16.384f

/* Exported types ------------------------------------------------------------*/

typedef enum {
    BSP_BMI088_OK = 0,
    BSP_BMI088_ERROR,
    BSP_BMI088_ID_ERROR,
    BSP_BMI088_TIMEOUT
} BSP_BMI088_Status_t;

/* Exported functions prototypes ---------------------------------------------*/

BSP_BMI088_Status_t BSP_BMI088_Init(void);
BSP_BMI088_Status_t BSP_BMI088_ACC_Init(void);
BSP_BMI088_Status_t BSP_BMI088_GYRO_Init(void);

void BSP_BMI088_ReadAccel(float *accel);
void BSP_BMI088_ReadGyro(float *gyro);
uint8_t BSP_BMI088_ReadID(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_BMI088_H */
