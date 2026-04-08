#include "cmsis_os2.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "main.h"
void Start_Serial_1_Task(void *argument)
{
  /* USER CODE BEGIN Start_Serial_1_Task */
  /* Infinite loop */
  char msg[50];
  
  uint32_t btn_count = 0;
  uint32_t data_count = 0;
  /* Infinite loop */
  for(;;)
  {
    // 从队列中获取数据
    osMessageQueueGet(BtnQueueHandle, &data_count, NULL, osWaitForever);
    btn_count++;
    osDelay(1000); // 模拟处理时间
    sprintf(msg, "\r\nButton pressed %d times, data: %d times \r\n", (int)btn_count,(int)data_count);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg)-1, HAL_MAX_DELAY);
      
    osDelay(10);
  }
  /* USER CODE END Start_Serial_1_Task */
}