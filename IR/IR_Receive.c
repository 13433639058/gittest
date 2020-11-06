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

////����ж϶�ʱ��	
//	TIM_Initure.Instance = TIM1;
//	TIM_Initure.Init.Period = 2400;
//	TIM_Initure.Init.Prescaler = 10000;
//	TIM_Initure.Init.CounterMode = TIM_COUNTERMODE_UP;
//	TIM_Initure.Init.ClockDivision = 0;
//	TIM_Initure.Init.RepetitionCounter = 0;
//	HAL_TIM_Base_Init(&TIM_BaseIniture);
	
//	TIM_CLOCK = (uint32_t)(HAL_RCC_GetPCLKFreq() / TIM_clock_set) - 1;   //����ϵͳʱ��Ϊ1Mhz
	TIM_Initure.Instance = TIM1;
	TIM_Initure.Init.Period = 2400;             //TIM_PEROD;                                 //10ms���ʱ��
	TIM_Initure.Init.Prescaler = 10000;         // TIM_CLOCK;                              //��ʱʱ��Ϊ(TIM_PEROD+1)/TIM_CLOCK
	TIM_Initure.Init.ClockDivision = 0;                                  //ʱ�ӷ�Ƶ
	TIM_Initure.Init.CounterMode = TIM_COUNTERMODE_UP;                   //�����ؼ���
	TIM_Initure.Init.RepetitionCounter = 0;
	TIM_Initure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_IC_Init(&TIM_Initure);
	
	//��ʼ��TIM1���벶�����
	TIM1_ICConfig.ICPolarity = TIM_ICPOLARITY_RISING;                           //�����ز�׽
	TIM1_ICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;                       //ӳ�䵽TI1��
	TIM1_ICConfig.ICPrescaler = TIM_ICPSC_DIV1;                                 //���������Ƶ������Ƶ
	TIM1_ICConfig.ICFilter = 0x03;                                              //IC4F=0003 8����ʱ��ʱ�������˲�
	HAL_TIM_IC_ConfigChannel(&TIM_Initure, &TIM1_ICConfig, TIM_CHANNEL_1);      //����TIM1ͨ��1
	HAL_TIM_IC_Start_IT(&TIM_Initure, TIM_CHANNEL_1);                           //��ʼ����ʱ�����벶��ͨ��
	__HAL_TIM_ENABLE_IT(&TIM_Initure, TIM_IT_UPDATE);                           //ʹ�ܸ����ж�
}

/* Init the low level hardware : GPIO, CLOCK, NVIC */
//��ʱ��1�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��1���
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
	HAL_TIM_IRQHandler(&TIM_Initure);//��ʱ�����ô�����
}

//�������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
uint8_t  IR_State = 0;
uint16_t Fall_Count = 0;    //�½��ؼ�������ֵ,���յ����ݵĵ�ʱ��Ϊ�͵�ƽ,û�н��յ����ݵ�ʱ��һֱ���ָߵ�ƽ
uint32_t Receive_Data = 0;  //������յ�������
uint8_t  Key_Count = 0;     //�������µĴ���



//��ʱ�����£�������жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM1)
	{
		if(IR_State & 0x80)  //�ϴ����ݱ������ˣ����Ѿ����յ�������
		{
			IR_State &= ~0x10;   //ȡ�������ز�׽��־
			if((IR_State & 0x0f) < 13)  //Ч��130msʱ��������ӣ���Ϊ����������ǳ���97.94ms�ĸߵ�ƽ����һ�������ʱ��Σ�
			{
				IR_State++;
			}
			else    //����130ms�������Ѿ��ɿ���������е�ֵ��ֻ��������λ�����ڰ����ж�
			{
				IR_State &= ~(1 << 7);
				IR_State &= 0xf0;        //��ռ�����
			}
		}
	}
}

//��ʱ�����벶���жϻص�����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if(htim->Instance==TIM1)
	{
		if(IR)   //�����ز���
		{
			TIM_RESET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1);                          //���ԭ�������ã�ȡ���½��ز�׽
			TIM_SET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);      //����Ϊ�½��ز�׽
			__HAL_TIM_SET_COUNTER(&TIM_Initure, 0);  //��ն�ʱ��
			IR_State |= 0x10;
		}
		else          //�½���
		{
			Fall_Count = HAL_TIM_ReadCapturedValue(&TIM_Initure, TIM_CHANNEL_1);            //��ȡ�ߵ�ƽ�Ĳ���ʱ��
			TIM_RESET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1);                          //���ԭ�������ã�ȡ���½��ز�׽
			TIM_SET_CAPTUREPOLARITY(&TIM_Initure,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);      //����Ϊ�����ز�׽
			if(IR_State & 0x10)  //�Ѿ����һ�θߵ�ƽ����
			{
				if(IR_State & 0x80)  //���յ�������
				{
					if(Fall_Count > 400 && Fall_Count < 700)           //���յ�0
					{
						Receive_Data <<= 1;
						Receive_Data |= 0;
					}
					else if(Fall_Count > 1500 && Fall_Count < 1800)    //���յ�1
					{
						Receive_Data <<= 1;
						Receive_Data |= 1;
					}
					else if(Fall_Count > 2350 && Fall_Count < 2650)    //������
					{
						Key_Count++;        //���´���+1
						IR_State &= 0xf0;   //��ռ�ʱ��
					}
				}
				else if(Fall_Count > 4300 && Fall_Count < 4700)  //��û�н��յ������룬���жϽ��յ����Ƿ���������
				{
					IR_State |= 0x80; //���յ�������
					Key_Count = 0;    //���֮ǰ�İ���������
				}
			}
			IR_State &= ~(1 << 4);  // ���־���Ա���
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
	
	if((Data_Byte != ~Data_OP_Byte) || ((IR_State & 0x80) == 0))  //���յ������ݴ�����߶���û�з�����
	{
		IR_State = 0;
	}
	
	return 0;
}
