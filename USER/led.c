#include  "led.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_it.h"
#include "main.h"
#include "sys.h"


void Run_Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

/***********RUN_LEDÒý½Å******************/
	GPIO_InitStructure.GPIO_Pin = RUN_LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(RUN_LED_PORT, &GPIO_InitStructure);   
}
void Led_On_Off(void)
{
	if(RUN_LED_TIME > TIME1S)
	{
		RUN_LED_TIME = 0;
		RUN_LED_OUT = ~RUN_LED_OUT;
	}
}
