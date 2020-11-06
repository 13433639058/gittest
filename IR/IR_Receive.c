#include "HW32l003_hal.h"
#include "HW32l003_hal_tim.h"
#include "IR_Receive.h"

uint32_t TIM_CLOCK;

TIM_HandleTypeDef TIM_Initure;

void IR_Init(void)
{
	__HAL_RCC_TIM1_CLK_ENABLE();
	TIM_HandleTypeDef TIM_BaseIniture;
	TIM_IC_InitTypeDef TIM1_ICConfig;

////溢出中断定时器	
//	TIM_Initure.Instance = TIM1;
//	TIM_Initure.Init.Period = 2400;
//	TIM_Initure.Init.Prescaler = 10000;
//	TIM_Initure.Init.CounterMode = TIM_COUNTERMODE_UP;
//	TIM_Initure.Init.ClockDivision = 0;
//	TIM_Initure.Init.RepetitionCounter = 0;
//	HAL_TIM_Base_Init(&TIM_BaseIniture);
	
//	TIM_CLOCK = (uint32_t)(HAL_RCC_GetPCLKFreq() / TIM_clock_set) - 1;   //配置系统时钟为1Mhz
	TIM_Initure.Instance = TIM1;
	TIM_Initure.Init.Period = 2400;             //TIM_PEROD;                                 //10ms溢出时间
	TIM_Initure.Init.Prescaler = 10000;         // TIM_CLOCK;                              //定时时间为(TIM_PEROD+1)/TIM_CLOCK
	TIM_Initure.Init.ClockDivision = 0;                                  //时钟分频
	TIM_Initure.Init.CounterMode = TIM_COUNTERMODE_UP;                   //上升沿计数
	TIM_Initure.Init.RepetitionCounter = 0;
	TIM_Initure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_IC_Init(&TIM_Initure);
	
	//初始化TIM1输入捕获参数
	TIM1_ICConfig.ICPolarity = TIM_ICPOLARITY_RISING;                           //上升沿捕捉
	TIM1_ICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;                       //映射到TI1上
	TIM1_ICConfig.ICPrescaler = TIM_ICPSC_DIV1;                                 //配置输入分频，不分频
	TIM1_ICConfig.ICFilter = 0x03;                                              //IC4F=0003 8个定时器时钟周期滤波
	HAL_TIM_IC_ConfigChannel(&TIM_Initure, &TIM1_ICConfig, TIM_CHANNEL_1);      //配置TIM1通道1
	HAL_TIM_IC_Start_IT(&TIM_Initure, TIM_CHANNEL_1);                           //初始化定时器输入捕获通道
	__HAL_TIM_ENABLE_IT(&TIM_Initure, TIM_IT_UPDATE);                           //使能更新中断
}

/* Init the low level hardware : GPIO, CLOCK, NVIC */
//定时器1底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器1句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef IR_GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	
	IR_GPIO_Initure.Pin = IR_Pin;
	IR_GPIO_Initure.Mode = GPIO_MODE_INPUT | GPIO_MODE_AF;
	IR_GPIO_Initure.Pull = GPIO_PULLUP;
	IR_GPIO_Initure.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	IR_GPIO_Initure.SlewRate = GPIO_SLEW_RATE_HIGH;
	IR_GPIO_Initure.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	HAL_GPIO_Init(IR_PORT, &IR_GPIO_Initure);
	
	HAL_NVIC_SetPriority(TIM1_IRQn, 1);
	HAL_NVIC_EnableIRQ(TIM1_IRQn);
}

void TIM1_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM_Initure);//定时器共用处理函数
}

//红外接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
uint8_t  IR_State = 0;
uint16_t Fall_Count = 0;    //下降沿计数器的值,接收到数据的的时候为低电平,没有接收到数据的时候一直保持高电平
uint32_t Receive_Data = 0;  //红外接收到的数据
uint8_t  Key_Count = 0;     //按键按下的次数



//定时器更新（溢出）中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM1)
	{
		if(IR_State & 0x80)  //上次数据被接受了，即已经接收到引导码
		{
			IR_State &= ~0x10;   //取消上升沿捕捉标志
			if((IR_State & 0x0f) < 13)  //效于130ms时，继续相加（因为连发码最后是持续97.94ms的高电平，给一定的误差时间段）
			{
				IR_State++;
			}
			else    //超过130ms，按键已经松开，清除所有的值，只保留第六位来用于按键判断
			{
				IR_State &= ~(1 << 7);
				IR_State &= 0xf0;        //清空计数器
			}
		}
	}
}

//定时器输入捕获中断回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	if(htim->Instance==TIM1)
	{
		if(IR)   //上升沿捕获
		{
			TIM_RESET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1);                          //清除原来的设置，取消下降沿捕捉
			TIM_SET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);      //设置为下降沿捕捉
			__HAL_TIM_SET_COUNTER(&TIM_Initure, 0);  //清空定时器
			IR_State |= 0x10;
		}
		else          //下降沿
		{
			Fall_Count = HAL_TIM_ReadCapturedValue(&TIM_Initure, TIM_CHANNEL_1);            //读取高电平的捕获时间
			TIM_RESET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1);                          //清除原来的设置，取消下降沿捕捉
			TIM_SET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);      //设置为上升沿捕捉
			if(IR_State & 0x10)  //已经完成一次高电平捕获
			{
				if(IR_State & 0x80)  //接收到引导码
				{
					if(Fall_Count > 400 && Fall_Count < 700)           //接收到0
					{
						Receive_Data <<= 1;
						Receive_Data |= 0;
					}
					else if(Fall_Count > 1500 && Fall_Count < 1800)    //接收到1
					{
						Receive_Data <<= 1;
						Receive_Data |= 1;
					}
					else if(Fall_Count > 2350 && Fall_Count < 2650)    //连发码
					{
						Key_Count++;        //按下次数+1
						IR_State &= 0xf0;   //清空计时器
					}
				}
				else if(Fall_Count > 4300 && Fall_Count < 4700)  //还没有接收到引导码，则判断接收到的是否是引导码
				{
					IR_State |= 0x80; //接收到引导码
					Key_Count = 0;    //清空之前的按键计数器
				}
			}
			IR_State &= ~(1 << 4);  // 清标志，以备用
		}
	}
}

int IR_Get(void)
{
	uint8_t First_Byte, First_OP_Byte, Data_Byte, Data_OP_Byte;
	First_Byte = Receive_Data >> 24;
	First_OP_Byte = (Receive_Data >> 16) & 0xff;
	Data_Byte = Receive_Data >> 8;
	Data_OP_Byte = Receive_Data;
	
	if(First_Byte == ~First_OP_Byte)
	{
		if(Data_Byte == ~Data_OP_Byte)
		{
			return Data_Byte;
		}
	}
	
	if((Data_Byte != ~Data_OP_Byte) || ((IR_State & 0x80) == 0))  //接收到的数据错误或者对面没有发送了
	{
		IR_State = 0;
	}
	
	return 0;
}
