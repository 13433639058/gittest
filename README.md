# gittest
the first test
#include "led.c"

void main()
{
    LED_Init();
    GPIO_ReadDataBit(GPIO, GPIO_Pin_4);
}