#include "cmsis_os2.h"
#include "gpio.h"
#include "main.h"
void StartTaskBtn(void *argument)
{
  /* USER CODE BEGIN StartTaskBtn */
  /* Infinite loop */

  uint32_t btn_flag = 0;
  uint32_t btn_count = 0;
  /* Infinite loop */
  for(;;)
  {
    if(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_SET)
    {
      btn_flag = 0; // 按键抬起
    }
      
    if(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET && btn_flag == 0) // 使用标志位防止一次按下计数多次
    {
      btn_flag = 1; // Button flag
      btn_count++;
      osMessageQueuePut(BtnQueueHandle, &btn_count, 0,osWaitForever);
    }
    osDelay(10);
  }
  /* USER CODE END StartTaskBtn */

}