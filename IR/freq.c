#include "HW32l003_hal.h"
#include "freq.h"

#define START_HIGH_TIME 9000*4.25
#define START_LOW_TIME  4500*4.25
#define ZERO_TIME       560*4.25
#define ONE_TIME        1680*4.25
#define END_TIME        9000*4.25
#define USER_ZERO_TIME  560*4.25

uint16_t Send_Flag;
uint32_t IR_User_Code = 0x00;

uint32_t TIM_CLOCK;
uint32_t IR_SendFlag, IR_SendCount;

void IR_FREQ_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();  //使能GPIOA时钟
	__HAL_RCC_TIM2_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_Initure;
	TIM_HandleTypeDef TIM_Initure;
	
	GPIO_Initure.Pin = IR_Pin;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT;
	GPIO_Initure.OpenDrain = GPIO_PUSHPULL;
	GPIO_Initure.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	GPIO_Initure.SlewRate = GPIO_SLEW_RATE_HIGH;
	GPIO_Initure.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	GPIO_Initure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(IR_PORT, &GPIO_Initure);
	
//	TIM_CLOCK = (uint32_t)(HAL_RCC_GetPCLKFreq() / TIM_clock_set) - 1;   //配置系统时钟为12Mhz
	
	TIM_Initure.Instance = TIM2;
	TIM_Initure.Init.Period = 2400;      //时钟频率为24MHz
	TIM_Initure.Init.Prescaler = 10000;  //定时时间为(TIM_PEROD+1)/TIM_CLOCK
	TIM_Initure.Init.ClockDivision = 0;
	TIM_Initure.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_Initure.Init.RepetitionCounter = 0;
	TIM_Initure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&TIM_Initure);
	
	HAL_NVIC_EnableIRQ(TIMx_IRQn);   //开启中断
	HAL_TIM_Base_Start_IT(&TIM_Initure); //以中断模式开启时钟
}

void IR_Send(uint16_t User_Code, uint16_t Send_Code)
{
	uint16_t i;
	uint16_t Send_Time;
	uint16_t Send_Code1, Send_Code2;
	uint16_t User_Code1, User_Code2;
	
//	Send_Flag = 1;
	Send_Flag = 0;
	
	Send_Time = START_HIGH_TIME;   //9ms high power
	Send_Flag = 1;
	while(Send_Time--);
	
	Send_Time = START_LOW_TIME;   //4.5ms low power
	Send_Flag = 0;
	while(Send_Time--);
	
	//用户码：0000 0000（自己定义）
	User_Code1 = User_Code;
	for(i = 0; i < 8; i++)  
	{
		Send_Flag = 0;
		
		Send_Time = ZERO_TIME;   //
		Send_Flag = 1;
		while(Send_Time--);
		
		if(User_Code1 & 0x01)  //低位在前
		{
			Send_Time = ONE_TIME;  //1
		}
		else
		{
			Send_Time = ZERO_TIME;   //0
		}
		Send_Flag = 0;
		while(Send_Time--);
		
		User_Code1 >>= 1;  //右移一位
	}
	
	//用户反码
	User_Code2 = User_Code;
	for(i = 0; i < 8; i++)
	{
		Send_Time = ZERO_TIME;
		Send_Flag = 1;
		while(Send_Time--);
		
		if(User_Code2 & 0x01)
		{
			Send_Time = ZERO_TIME;
		}
		else
		{
			Send_Time = ONE_TIME;
		}
		Send_Flag = 0;
		while(Send_Time--);
		
		User_Code2 >>= 1;  //右移一位
	}
	
	//键码
	Send_Code1 = Send_Code;
	for(i = 0; i < 8; i++)
	{
		Send_Time = ZERO_TIME;
		Send_Flag = 1;
		while(Send_Time--);
		
		if(Send_Code1 & 0x01)
		{
			Send_Time = ONE_TIME;
		}
		else
		{
			Send_Time = ZERO_TIME;
		}
		Send_Flag = 0;
		while(Send_Time--);
		
		Send_Code1 >>= 1;  //右移一位
	}
	
	//键码反码
	Send_Code2 = Send_Code;
	for(i = 0; i < 8; i++)
	{
		Send_Time = ZERO_TIME;
		Send_Flag = 1;
		while(Send_Time--);
		
		if(Send_Code2 & 0x01)
		{
			Send_Time = ZERO_TIME;
		}
		else
		{
			Send_Time = ONE_TIME;
		}
		Send_Flag = 0;
		while(Send_Time--);
		
		Send_Code2 >>= 1;  //右移一位
	}
	
	for(i = 0; i < 14; i++)
	{
		Send_Time = START_LOW_TIME;   //4.5ms low power
		Send_Flag = 0;
		while(Send_Time--);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  //13us进入中断一次 TIM2_IRQHandler
{
	if(Send_Flag == 1)
	{
		HAL_GPIO_ReadPin(IR_PORT, IR_Pin)?HAL_GPIO_WritePin(IR_PORT, IR_Pin, GPIO_PIN_RESET):HAL_GPIO_WritePin(IR_PORT, IR_Pin, GPIO_PIN_SET);
	}
}
