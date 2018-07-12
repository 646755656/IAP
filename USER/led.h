#ifndef __LED_H
#define __LED_H


#define RUN_LED_PORT GPIOC
#define RUN_LED_PIN GPIO_Pin_13
#define RUN_LED_OUT PCout(13)

void Run_Led_Init(void);
void Led_On_Off(void);
#endif
