#include "main.h"
#include "work.h"
#include "sys.h"
#include "delay.h"
#include "uart.h"
#include "timer.h"
#include "stmflash.h"
#include "iap.h"
#include "w25qxx.h"
#include "watchdog.h"


 int main(void)
 {	
 #if UPDATA_EXIT
 	u8 UpdateFlag = 0;
 #else
 	u16 UpdateFlag = 0;
#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//�����ж����ȼ�����Ϊ��2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	RCC_Clock_Config();
	delay_init();	    //��ʱ������ʼ��	  
	Run_Led_Init();
	Timer_Config();
	UART_Init();
	
	W25QXX_Init();

	//��ȡ������־
 #if UPDATA_EXIT
	W25QXX_Read(ADDR_UPDATA_SUCCESS_FLAG, &UpdateFlag, 1);
#else
	STMFLASH_Read(ADDR_UPDATA_SUCCESS_FLAG, &UpdateFlag, 1);
#endif

	if(UpdateFlag == 0x55)		//����
		Updata_Program();
	Iap_Load_App(ADDR_APP_RUN);

	//����ʧ��
	Read_Com_Addr();

	//WDG_Init();
	while(1)//IAP
	{
		Led_Blink();//����ָʾ��

		Send_Heart_Pack_Data();

		if((COM1.jg_time >= TIME500MS) && (COM1.rcv_len > 10))		//���յ���������
		{
			 if(COM1.rcv_buff[0] == 0x3c)	//����
			{
				Rec_Updata_Data(COM1.rcv_buff,COM1.rcv_len);
				UART1_Clear();
			}
			else
				UART1_Clear();
		}

		if(UpdateSuccessAddrFlag == 0x55)
		{
			if(UpdataRestartTimeCs > TIME1MIN)	// 1min������
				Soft_Reset();
		}
	}
 }

