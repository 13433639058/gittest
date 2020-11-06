#include "uart.h"
#include "HW32l003_hal.h"

GPIO_InitTypeDef GPIO_Initure;
UART_HandleTypeDef UART_Initure;

void UART_Init(void)
{
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_UART1_CLK_ENABLE();
	
	/********PD5->TX********/
	GPIO_Initure.Pin = GPIO_PIN_5;
	GPIO_Initure.Mode = GPIO_MODE_AF;
	GPIO_Initure.Alternate = GPIO_AF5_UART1_TXD;
	GPIO_Initure.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	GPIO_Initure.SlewRate = GPIO_SLEW_RATE_HIGH;
	GPIO_Initure.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.OpenDrain = GPIO_PUSHPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);
	
	/********PD6->RX********/
	GPIO_Initure.Pin = GPIO_PIN_6;
	GPIO_Initure.Mode = GPIO_MODE_AF;
	GPIO_Initure.Alternate = GPIO_AF5_UART1_RXD;
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);
	
	__HAL_RCC_UART1_CLK_ENABLE();
	UART_Initure.Instance = UART1;
	UART_Initure.Init.BaudRate = 9600;
	UART_Initure.Init.BaudDouble = UART_BAUDDOUBLE_DISABLE;
	UART_Initure.Init.WordLength = UART_WORDLENGTH_8B;           //�ֽڳ���Ϊ8bit
	UART_Initure.Init.Parity = UART_PARITY_NONE;                 //����żУ��
	UART_Initure.Init.Mode = UART_MODE_TX_RX;                    //��������� 
	HAL_UART_Init(&UART_Initure);
}

int fputc(int ch, FILE *f) 
{
	HAL_UART_Transmit(&UART_Initure, (uint8_t*)&ch, 1, 2);    //ͨ��������ʾEDID��Ϣ
	return 0;
}
