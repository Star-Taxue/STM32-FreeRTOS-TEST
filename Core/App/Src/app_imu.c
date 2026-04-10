/**
 * @file    app_imu.c
 * @brief   IMU应用层实现
 * @author  STM32 Team
 * @date    2026-04-09
 * @version V1.0
 */

#include "Inc/app_imu.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

extern osMessageQueueId_t IMU_DataHandle;

void APP_IMU_Init(void)
{
    char msg[50];
    
    if (BSP_BMI088_Init() != BSP_BMI088_OK) {
        sprintf(msg, "BMI088 Init Failed!\r\n");
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        return;
    }
    
    BSP_IMU_Init();
    sprintf(msg, "BMI088 and IMU Init Success!\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void Start_IMU_Task(void *argument)
{
    float accel[3] = {0};
    float gyro[3] = {0};
    euler_angle_t angle;
    IMU_Data_t imuData;
    char msg[64];
    const float dt = 0.02f;
    
    (void)argument;
    
    for (;;) {
        BSP_BMI088_ReadAccel(accel);
        BSP_BMI088_ReadGyro(gyro);
        
        BSP_IMU_Update(accel, gyro, dt);
        BSP_IMU_GetAngle(&angle);
        
        imuData.accel[0] = accel[0];
        imuData.accel[1] = accel[1];
        imuData.accel[2] = accel[2];
        imuData.gyro[0] = gyro[0];
        imuData.gyro[1] = gyro[1];
        imuData.gyro[2] = gyro[2];
        imuData.euler[0] = angle.roll;
        imuData.euler[1] = angle.pitch;
        imuData.euler[2] = angle.yaw;
        imuData.timestamp = osKernelGetTickCount();
        
        if (IMU_DataHandle != NULL) {
            osMessageQueuePut(IMU_DataHandle, &imuData, 0, 0);
        }
        
        sprintf(msg, "Roll: %d, Pitch: %d, Yaw: %d\r\n",
                (int)angle.roll*10, (int)angle.pitch, (int)angle.yaw);
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        
        osDelay(20);
    }
}
