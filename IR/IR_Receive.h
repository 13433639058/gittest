#ifndef __IR_RECEIVE_H__
#define __IR_RECEIVE_H__

#define IR_PORT            GPIOA
#define IR_Pin             GPIO_PIN_2
#define TIM_clock_set      1000   //1KHz
#define TIM_PEROD          (10 - 1)    //(TIM_PEROD + 1)/TIM_clock_set 
#define TIMx_IRQn          TIM1_IRQn 
#define IR                 HAL_GPIO_ReadPin(IR_PORT, IR_Pin)

void IR_Init(void);
int IR_Get(void);

#endif
