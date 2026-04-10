/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "usart.h"
#include "gpio.h"
#include <stdint.h>
#include <stdio.h>
#include "Inc/app_imu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */
/* Definitions for LED_Task */
osThreadId_t LED_TaskHandle;
const osThreadAttr_t LED_Task_attributes = {
  .name = "LED_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Serial_1_Task */
osThreadId_t Serial_1_TaskHandle;
const osThreadAttr_t Serial_1_Task_attributes = {
  .name = "Serial_1_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Btn_Task */
osThreadId_t Btn_TaskHandle;
const osThreadAttr_t Btn_Task_attributes = {
  .name = "Btn_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for IMU_Task */
osThreadId_t IMU_TaskHandle;
const osThreadAttr_t IMU_Task_attributes = {
  .name = "IMU_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for BtnQueue */
osMessageQueueId_t BtnQueueHandle;
const osMessageQueueAttr_t BtnQueue_attributes = {
  .name = "BtnQueue"
};
/* Definitions for IMU_Data */
osMessageQueueId_t IMU_DataHandle;
const osMessageQueueAttr_t IMU_Data_attributes = {
  .name = "IMU_Data"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Start_LED_Task(void *argument);
extern void Start_Serial_1_Task(void *argument);
extern void StartTaskBtn(void *argument);
extern void Start_IMU_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of BtnQueue */
  BtnQueueHandle = osMessageQueueNew (16, sizeof(uint32_t), &BtnQueue_attributes);

  /* creation of IMU_Data */
  IMU_DataHandle = osMessageQueueNew (16, sizeof(IMU_Data_t*), &IMU_Data_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LED_Task */
  LED_TaskHandle = osThreadNew(Start_LED_Task, NULL, &LED_Task_attributes);

  /* creation of Serial_1_Task */
  Serial_1_TaskHandle = osThreadNew(Start_Serial_1_Task, NULL, &Serial_1_Task_attributes);

  /* creation of Btn_Task */
  Btn_TaskHandle = osThreadNew(StartTaskBtn, NULL, &Btn_Task_attributes);

  /* creation of IMU_Task */
  IMU_TaskHandle = osThreadNew(Start_IMU_Task, NULL, &IMU_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Start_LED_Task */
/**
  * @brief  Function implementing the LED_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Start_LED_Task */
__weak void Start_LED_Task(void *argument)
{
  /* USER CODE BEGIN Start_LED_Task */
  /* Infinite loop */
  for(;;)
  {
  }
  /* USER CODE END Start_LED_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

