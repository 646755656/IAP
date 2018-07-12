/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "uart.h"
#include "timer.h"


unsigned int RUN_LED_TIME = 0;
unsigned int COM1_TIME_OUT = 0;
unsigned int COM2_TIME_OUT = 0;
unsigned int COM3_TIME_OUT = 0;
unsigned int COM4_TIME_OUT = 0;
unsigned int COM5_TIME_OUT = 0;

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
struct com_define COM1;
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	unsigned char rc=0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //如果是接收中断
	{     
		rc = USART_ReceiveData(USART1);
		COM1.rcv_buff[COM1.rcv_len] = rc;
		COM1.rcv_len++;
		if(COM1.rcv_len >= RCV_BUFF_LENGH) COM1.rcv_len = 0;

		COM1.jg_time = 0;
		COM1.rcv_flag = 1;

		COM1_TIME_OUT = 0;
	}
}


/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
struct com2_define COM2;
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	unsigned char rc=0;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //如果是接收中断
	{     
		rc = USART_ReceiveData(USART2);
		COM2.rcv_buff[COM2.rcv_len] = rc;
		COM2.rcv_len++;
		if(COM2.rcv_len >= RCV_BUFF_LENGH) COM2.rcv_len = 0;

		COM2.jg_time = 0;
		COM2.rcv_flag = 1;

		COM2_TIME_OUT = 0;
	}
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
struct com_define COM3;
void USART3_IRQHandler(void)
{
	unsigned char rc=0;//,rcn=0;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //如果是接收中断
	{     
		rc = USART_ReceiveData(USART3);
		COM3.rcv_buff[COM3.rcv_len] = rc;
		COM3.rcv_len++;
		if(COM3.rcv_len >= RCV_BUFF_LENGH) COM3.rcv_len = 0;

		COM3.jg_time = 0;
		COM3.rcv_flag = 1;
		COM3_TIME_OUT = 0;
	}
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
struct com2_define COM4;
void UART4_IRQHandler(void)
{
	unsigned char rc=0;//,rcn=0;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //如果是接收中断
	{     
		rc = USART_ReceiveData(UART4);
		COM4.rcv_buff[COM4.rcv_len] = rc;
		COM4.rcv_len++;
		if(COM4.rcv_len >= RCV2_BUFF_LENGH) COM4.rcv_len = 0;

		COM4.jg_time = 0;
		COM4.rcv_flag = 1;
	}
	
}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
struct com_define COM5;
void UART5_IRQHandler(void)
{
	unsigned char rc=0;//,rcn=0;
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //如果是接收中断
	{     
		rc = USART_ReceiveData(UART5);
		COM5.rcv_buff[COM5.rcv_len] = rc;
		COM5.rcv_len++;
		if(COM5.rcv_len >= RCV_BUFF_LENGH) COM5.rcv_len = 0;

		COM5.jg_time = 0;
		COM5.rcv_flag = 1;
	}
	
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{ 
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		RUN_LED_TIME ++;

		COM1_TIME_OUT ++;
		COM2_TIME_OUT ++;

		COM1.jg_time ++;
		COM2.jg_time ++;
		COM3.jg_time ++;
		COM4.jg_time ++;
		COM5.jg_time ++;
	}    
   
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
  
}



void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
