/**
 * @file    bsp_bmi088.c
 * @brief   BMI088传感器BSP层实现
 * @author  STM32 Team
 * @date    2026-04-09
 * @version V1.0
 */

#include "Inc/bsp_bmi088.h"
#include "main.h"
#include "spi.h"
#include <string.h>

static void delay_nop(uint32_t count)
{
    for(volatile uint32_t i = 0; i < count; i++) {
        __NOP();
    }
}

static uint8_t bmi088_acc_read_byte(uint8_t reg)
{
    uint8_t tx_data[2] = {reg | 0x80, 0x00};
    uint8_t rx_data[2] = {0};
    
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_RESET);
    delay_nop(100);
    
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 100);
    
    delay_nop(100);
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_SET);
    delay_nop(100);
    
    return rx_data[1];
}

static void bmi088_acc_write_byte(uint8_t reg, uint8_t data)
{
    uint8_t tx_data[2] = {reg & 0x7F, data};
    
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_RESET);
    delay_nop(100);
    
    HAL_SPI_Transmit(&hspi1, tx_data, 2, 100);
    
    delay_nop(100);
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_SET);
    osDelay(2);
}

static uint8_t bmi088_gyro_read_byte(uint8_t reg)
{
    uint8_t tx_data[2] = {reg | 0x80, 0x00};
    uint8_t rx_data[2] = {0};
    
    DRV_SPI1_GYRO_CS_Ctrl(GPIO_PIN_RESET);
    delay_nop(100);
    
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 100);
    
    delay_nop(100);
    DRV_SPI1_GYRO_CS_Ctrl(GPIO_PIN_SET);
    delay_nop(100);
    
    return rx_data[1];
}

static void bmi088_gyro_write_byte(uint8_t reg, uint8_t data)
{
    uint8_t tx_data[2] = {reg & 0x7F, data};
    
    DRV_SPI1_GYRO_CS_Ctrl(GPIO_PIN_RESET);
    delay_nop(100);
    
    HAL_SPI_Transmit(&hspi1, tx_data, 2, 100);
    
    delay_nop(100);
    DRV_SPI1_GYRO_CS_Ctrl(GPIO_PIN_SET);
    osDelay(2);
}

BSP_BMI088_Status_t BSP_BMI088_Init(void)
{
    osDelay(100);
    
    if (BSP_BMI088_ACC_Init() != BSP_BMI088_OK) {
        return BSP_BMI088_ERROR;
    }
    
    if (BSP_BMI088_GYRO_Init() != BSP_BMI088_OK) {
        return BSP_BMI088_ERROR;
    }
    
    return BSP_BMI088_OK;
}

BSP_BMI088_Status_t BSP_BMI088_ACC_Init(void)
{
    uint8_t id = 0;
    int retry = 0;
    
    // SPI模式激活流程
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_SET);
    osDelay(10);
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_RESET);
    delay_nop(100);
    
    uint8_t tx_data[2] = {BMI088_ACC_CHIP_ID | 0x80, 0x00};
    uint8_t rx_data[2] = {0};
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 100);
    
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_SET);
    osDelay(10);
    
    // 软复位
    bmi088_acc_write_byte(BMI088_ACC_SOFTRESET, 0xB6);
    osDelay(50);
    
    // 读取ID
    do {
        id = bmi088_acc_read_byte(BMI088_ACC_CHIP_ID);
        osDelay(10);
        retry++;
    } while (id != BMI088_ACC_CHIP_ID_VALUE && retry < 10);
    
    if (id != BMI088_ACC_CHIP_ID_VALUE) {
        return BSP_BMI088_ID_ERROR;
    }
    
    osDelay(1);
    
    bmi088_acc_write_byte(BMI088_ACC_PWR_CONF, 0x00);
    osDelay(5);
    
    bmi088_acc_write_byte(BMI088_ACC_CONF, BMI088_ACC_ODR_100HZ);
    osDelay(5);
    
    bmi088_acc_write_byte(BMI088_ACC_RANGE, BMI088_ACC_RANGE_6G);
    osDelay(5);
    
    bmi088_acc_write_byte(BMI088_ACC_PWR_CTRL, 0x04);
    osDelay(50);
    
    return BSP_BMI088_OK;
}

BSP_BMI088_Status_t BSP_BMI088_GYRO_Init(void)
{
    osDelay(1000);
    
    uint8_t id = bmi088_gyro_read_byte(BMI088_GYRO_CHIP_ID);
    if (id != BMI088_GYRO_CHIP_ID_VALUE) {
        return BSP_BMI088_ID_ERROR;
    }
    
    bmi088_gyro_write_byte(BMI088_GYRO_RANGE, BMI088_GYRO_RANGE_2000DPS);
    osDelay(10);
    
    bmi088_gyro_write_byte(BMI088_GYRO_BANDWIDTH, BMI088_GYRO_ODR_1000HZ);
    osDelay(10);
    
    return BSP_BMI088_OK;
}

void BSP_BMI088_ReadAccel(float *accel)
{
    uint8_t tx_data[7] = {BMI088_ACC_X_LSB | 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t rx_data[7] = {0};
    
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_RESET);
    delay_nop(100);
    
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 7, 100);
    
    DRV_SPI1_ACC_CS_Ctrl(GPIO_PIN_SET);
    
    int16_t raw_x = (int16_t)((rx_data[1] << 8) | rx_data[2]);
    int16_t raw_y = (int16_t)((rx_data[3] << 8) | rx_data[4]);
    int16_t raw_z = (int16_t)((rx_data[5] << 8) | rx_data[6]);

    accel[0] = (float)raw_y / BMI088_ACC_SENSITIVITY;
    accel[1] = (float)raw_z / BMI088_ACC_SENSITIVITY;
    accel[2] = (float)raw_x / BMI088_ACC_SENSITIVITY;
}

void BSP_BMI088_ReadGyro(float *gyro)
{
    uint8_t tx_data[7] = {BMI088_GYRO_RATE_X_LSB | 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t rx_data[7] = {0};
    
    DRV_SPI1_GYRO_CS_Ctrl(GPIO_PIN_RESET);
    delay_nop(100);
    
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 7, 100);
    
    DRV_SPI1_GYRO_CS_Ctrl(GPIO_PIN_SET);
    
    int16_t raw_x = (int16_t)((rx_data[2] << 8) | rx_data[1]);
    int16_t raw_y = (int16_t)((rx_data[4] << 8) | rx_data[3]);
    int16_t raw_z = (int16_t)((rx_data[6] << 8) | rx_data[5]);
    
    gyro[0] = (float)raw_x / BMI088_GYRO_SENSITIVITY;
    gyro[1] = (float)raw_y / BMI088_GYRO_SENSITIVITY;
    gyro[2] = (float)raw_z / BMI088_GYRO_SENSITIVITY;
}
