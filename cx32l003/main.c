#include "HW32l003_hal.h"
#include "uart.h"
#include "i2c.h"
#include "IR_Receive.h"

#define device_address 0xA0
#define register_address 0x00
#define EDID_BUF_LEN 128
extern UART_HandleTypeDef UART_Initure;
extern I2C_HandleTypeDef I2C_Initure;

uint32_t tick, rDx = 0, state = 0;
uint8_t EDID_BUF[EDID_BUF_LEN];
uint8_t RX_BUF[1];

int main()
{
	UART_Init();
	I2C_Init();
	IR_Init();
	
	if(SysTick_Config(SystemCoreClock / 1000))
	{
		while(1);
	}
	
	while(1)
	{
		HAL_UART_Receive(&UART_Initure, RX_BUF, 128, 20);  //接收到串口传输过来的EDID数据
		HAL_I2C_Master_Transmit(&I2C_Initure, device_address, EDID_BUF, 128);  //将接收到的EDID数据通过I2C传输给另一个单片机
		if(tick > 20)
		{
			
		}
	}
}

void HAL_SYSTICK_Callback(void)
{
	tick++;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint32_t c;
	c = RX_BUF[0];
	
	switch(state)
	{
		case 0:
			if(c == 0xaa)
			{
				rDx = 0;
				state = 1;
			}
			break;
		case 1:
			if(c == 0x55)
			{
				state = 2;
				rDx = 0;
			}
			else
			{
				EDID_BUF[rDx] = c;
				if(rDx < EDID_BUF_LEN)
				rDx++;
				else
				state = 0;
			}
			break;
		case 2:
			break;
		default:
			break;
	}
}
