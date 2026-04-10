/**
 * @file    app_imu.h
 * @brief   IMU应用层接口
 * @author  STM32 Team
 * @date    2026-04-09
 * @version V1.0
 *
 * @description
 * APP层IMU模块，负责整合BSP层的BMI088驱动和姿态解算，
 * 通过FreeRTOS任务和队列与系统其他模块通信。
 * 用户需要在freertos.c中创建任务和队列。
 */

#ifndef APP_IMU_H
#define APP_IMU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Inc/bsp_bmi088.h"
#include "Inc/bsp_imu.h"
#include "cmsis_os2.h"

/* Exported types ------------------------------------------------------------*/

typedef struct {
    float accel[3];
    float gyro[3];
    float euler[3];
    uint32_t timestamp;
} IMU_Data_t;

/* Exported variables --------------------------------------------------------*/

extern osMessageQueueId_t IMU_DataHandle;

/* Exported functions prototypes ---------------------------------------------*/

void APP_IMU_Init(void);
void Start_IMU_Task(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* APP_IMU_H */
