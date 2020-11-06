#ifndef __FREQ_H__
#define __FREQ_H__

#define IR_PORT          GPIOA
#define IR_Pin           GPIO_PIN_3

#define TIMx TIM2
#define TIM_clock_set    12000000          //12Mhz，时钟为1us
#define TIM_PEROD        12*(13-1)           //最终定时时间为（TIM_PEROD+1）/TIM_clock_set
#define TIMx_IRQn        TIM2_IRQn
//#define IR_FLAG_ON       1
//#define IR_FLAG_OFF      0


void IR_FREQ_Init(void);
void IR_Send(uint16_t User_Code, uint16_t Send_Code);

#endif
