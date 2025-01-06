#include "main.h"

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_PWREx_DisableUCPDDeadBattery();
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart) { }
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) { }
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) { }
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) { }
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) { }
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base) { }

