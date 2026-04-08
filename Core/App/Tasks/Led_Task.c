#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "main.h"
void Start_LED_Task(void *argument)
{
  /* USER CODE BEGIN Start_LED_Task */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    osDelay(200);
  }
  /* USER CODE END Start_LED_Task */
}