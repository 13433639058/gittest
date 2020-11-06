#include "HW32l003_hal.h"
#include "uart.h"
#include "i2c.h"
#include "freq.h"

#define device_address 0xA0
#define register_address 0x00
#define EDID_BUF_LEN 128

uint32_t tick, tick1;
uint8_t EDID_BUF[EDID_BUF_LEN];
extern UART_HandleTypeDef UART_Initure;
extern I2C_HandleTypeDef I2C_Initure;

void Error_Handler(void);
void SystemClock_Config(void);

int main()
{
	HAL_Init();
	SystemClock_Config();
	
	UART_Init();
	I2C_Init();
	IR_FREQ_Init();
	
	if(SysTick_Config(SystemCoreClock / 1000000))
	{
		while(1);
	}
	
	while(1)
	{
		HAL_I2C_Master_Receive(&I2C_Initure, device_address, EDID_BUF, 128);    //从另一个芯片接收ic传输过来的EDID数据
		if(tick > 20000)
		{
			tick = 0;
			HAL_UART_Transmit(&UART_Initure, EDID_BUF, 128, 20);
		}
		
		IR_Send(0xff, 0xff);
		
//		if(tick1 > 13)  //13us翻转一次，产生38kHz频率
//		{
//			tick1 = 0;
//			HAL_GPIO_ReadPin(IR_PORT, IR_Pin)?HAL_GPIO_WritePin(IR_PORT, IR_Pin, GPIO_PIN_RESET):HAL_GPIO_WritePin(IR_PORT, IR_Pin, GPIO_PIN_SET);
//		}
		
	}
}

void HAL_SYSTICK_Callback(void)
{
	tick++;
	tick1++;
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};	
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HIRC;
  RCC_OscInitStruct.HIRCState = RCC_HIRC_ON;
  RCC_OscInitStruct.HIRCCalibrationValue = RCC_HIRCCALIBRATION_24M;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
	
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HIRC;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APBCLKDivider = RCC_PCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


