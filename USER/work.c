#include "work.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_it.h"
#include "sys.h"
#include "stmflash.h"
#include "iap.h"
#include "main.h"
#include "w25qxx.h"
#include "watchdog.h"
#include "uart.h"

u8 SUPER_STATION_NUMMBER[8]={'8','0','8','0','8','0','8','0'};		// 超级用户编码
struct ComAddr_define ComAddr;
u8 WaterTerminalAddr[5] = {0};			//用于水资源协议终端地址上传


u32 RecTotalLen = 0;		//接收总长度
u16 RecTotalNum = 0;		//接收总次数
u16 RecOneLen = 0;			//单次接收数据长度
u16 RecLastLen = 0;		//接收最后一帧数据长度
u16 RecIngNum = 0;		//正在接收的升级程序条数

u32 UpdataRestartTimeCs = 0;
u32 HeartDataTimeCs = 0;			//心跳包计时

#if UPDATA_EXIT
	u8 UpdateSuccessAddrFlag = 0;		//升级成功标志
#else
	u16 UpdateSuccessAddrFlag = 0;		//升级成功标志
#endif


void Updata_Program()
{
	u8 i = 0,totalnum = 0,flag = 0;
	u16 leftlen = 0;
	u32 OverAddr = 0;		//升级程序结束地址
 #if UPDATA_EXIT
	u8 upflag = 0, rbuf[1024] = {0};
#else
	u16 upflag = 0, rbuf[512] = {0};
#endif
	Read_Updata_Pro_Data();
	totalnum = RecTotalLen / 1024;
	flag = RecTotalLen % 1024;
	if(flag != 0)
		totalnum++;

	//先判断最后2个字节是否为结束标志
	OverAddr = ADDR_PRO_DOWNLOAD + RecTotalLen;
	W25QXX_Read(OverAddr, rbuf, 2);
	if((rbuf[0] != PRO_COMPLETE_DATA) || (rbuf[1] != PRO_COMPLETE_DATA))		//验证最后2字节
		return;
	

	for(i = 0; i < totalnum; i++)
	{
		if(i < totalnum - 1)
		{
 #if UPDATA_EXIT
			W25QXX_Read(ADDR_PRO_DOWNLOAD + i * 1024, rbuf, 1024);
			iap_write_appbin(ADDR_APP_RUN + i * 1024, rbuf, 1024);
#else
			STMFLASH_Read(ADDR_PRO_DOWNLOAD + i * 1024, rbuf, 512);
			STMFLASH_Write(ADDR_APP_RUN+ i * 1024, rbuf, 512);
#endif
		}
		else
		{
			leftlen = RecTotalLen - i * 1024;
			leftlen += 2;		//添加2字节的结束标志，用于验证程序的完整
 #if UPDATA_EXIT
			W25QXX_Read(ADDR_PRO_DOWNLOAD + i * 1024, rbuf, leftlen);
			iap_write_appbin(ADDR_APP_RUN + i * 1024, rbuf, leftlen);
#else
			STMFLASH_Read(ADDR_PRO_DOWNLOAD + i * 1024, rbuf, leftlen / 2);
			STMFLASH_Write(ADDR_APP_RUN+ i * 1024, rbuf, leftlen / 2);
#endif
		}
	}
 #if UPDATA_EXIT
	W25QXX_Write(ADDR_UPDATA_SUCCESS_FLAG, &upflag, 1);	//清0
#else
	STMFLASH_Write(ADDR_UPDATA_SUCCESS_FLAG, &upflag, 1);	//清0
#endif

	Iap_Load_App(ADDR_APP_RUN);	//跳转运行
}


void Rec_Updata_Data(u8 *buff,u16 length)
{
	u8 i = 0,flag = 1,rsu[2]={0},CodeLen = 0,CodePos = 0;
	u16 j=0,pos = 0,datalen=0,LenDT = 0, RecNum = 0;
	u8 OrdCode = 0,OrdVer = 0,OrdDir = 0,OrdMark = 0, OrdFrame = 0;
	u8 databuf[1100] = {0};

	WatchDog();
	if(buff[pos++] != ORDER_START_CODE)		//协议头
		return;
	if(buff[pos++] != ORDER_VERSION)			//协议版本
	{
		rsu[0] = COMMU_ERROR_NONSUPORT;			// 不支持指令 
		return;
	}
	if(buff[length - 1] != ORDER_END_CODE)		//协议尾
		return;

	LenDT = change_HEX_to_DEC(&buff[pos], 2);	//长度
	if(LenDT != length)
		return;
	pos += 2;

	CodeLen = buff[pos++];
	if(CodeLen != ORDER_STATION_LENGTH)	//终端编码长度
	{
		if(CodeLen != 8)		//超级编码的长度
		{
			Respone_Upper_Order(OrdCode,0);
			return;
		}
	}

	flag = 1;
	CodePos = pos;		//终端编码起始位置
	for(i=0;i<ORDER_STATION_LENGTH;i++)
	{
		if(buff[pos++] != ComAddr.TerminalAddr[i])
		{
			flag = 0;
			break;
		}
	}
	if(flag ==0)
	{
		flag = 1;
		pos = CodePos;
		for(i=0;i<8;i++)
		{
			if(buff[pos++] != SUPER_STATION_NUMMBER[i])
			{
				flag = 0;
				break;
			}
		}
	}
	if(flag == 0)
	{
		Respone_Upper_Order(OrdCode,0);
		return;
	}
	WatchDog();

	OrdFrame = buff[pos++];		//帧号
	OrdDir = buff[pos++];			//指令方向
	if((OrdDir != ORDER_DIR_MD_SEND) && (OrdDir != ORDER_DIR_MD_RESPOND))		//应答方向
	{
		Respone_Upper_Order(OrdCode,0);
		return;
	}
	
	OrdMark = buff[pos++];
	if(OrdMark != ORDER_CODE)			//0xA1
		return;
	OrdCode = buff[pos++];					//指令码
	OrdVer = buff[pos++];						//指令版本
	
	CRC16(&buff[1],LenDT - 4);		// 3c之后到crc之前校验
	if((buff[LenDT -3] != uchCRCLo) || (buff[LenDT-2] != uchCRCHi))
		return;
	
	datalen = LenDT - pos - 3;		//指令版本之后的数据
	for(j = 0; j < datalen; j++)
		databuf[j] = buff[pos+j];

	WatchDog();
	//解析数据区
	switch(OrdCode)
	{
		case 0x30:		//连接指令
			if(OrdVer != ORDER_CODE_WRITE_PARAM_VER)
			{
				Respone_Upper_Order(OrdCode,0);
				return;
			}
			pos = 0;
			RecTotalLen = (u32)change_HEX_to_DEC(&databuf[pos], 4);	//数据总长度
			pos += 4;			
			RecTotalNum = databuf[pos++];		//接收总次数
			RecOneLen = (u16)change_HEX_to_DEC(&databuf[pos], 2);	//单次接收数据长度
			pos += 2;
			RecLastLen = (u16)change_HEX_to_DEC(&databuf[pos], 2);	//最后一帧数据长度
			pos += 2;

			RecIngNum = 0;
			Write_Updata_Pro_Data();		//保存数据
//			rsu[0] = 1;
//			return_Comm_Info(OrdFrame, OrdMark, OrdCode, OrdVer, rsu, 1);
			Respone_Upper_Order(OrdCode,1);
			
			break;
		case 0x32:
			if(OrdVer != ORDER_CODE_WRITE_PARAM_VER)
			{
				Respone_Upper_Order(OrdCode,1);
				return;
			}
			pos = 0;
			RecNum = (u16)change_HEX_to_DEC(&databuf[pos], 2);
			pos += 2;			//正在接收的条数
			if(RecNum != (RecIngNum + 1))
			{
				if(RecNum != RecIngNum)
				{
					Respone_Upper_Order(OrdCode,0);
					return;
				}
//				else
//				{
//					Respone_Upper_Order(OrdCode,1);
//					return;
//				}
			}
			RecIngNum = RecNum;
				
			if(RecIngNum < RecTotalNum)		
			{
				datalen = (u16)change_HEX_to_DEC(&databuf[pos], 2);
				pos += 2;
				if(datalen != RecOneLen)		//数据长度不对
				{
//					rsu[0] = 0;
//					return_Comm_Info(OrdFrame, OrdMark, OrdCode, OrdVer, rsu, 1);
					Respone_Upper_Order(OrdCode,0);
					return;
				}
				WatchDog();
 #if UPDATA_EXIT
				W25QXX_Write(ADDR_PRO_DOWNLOAD + (RecIngNum - 1) * RecOneLen, &databuf[pos], datalen);
//				W25QXX_Read(ADDR_PRO_DOWNLOAD + (RecIngNum - 1) * RecOneLen, &databuf[pos], datalen);
#else
				iap_write_appbin(ADDR_PRO_DOWNLOAD + (RecIngNum - 1) * RecOneLen, &databuf[pos], datalen);
#endif
				Write_Updata_Pro_Data();
				Respone_Upper_Order(OrdCode,1);
			}
			else									//接收到最后一条
			{
				datalen = change_HEX_to_DEC(&databuf[pos], 2);
				pos += 2;
				if(datalen != RecLastLen + 2)		//数据长度不对
				{
					Respone_Upper_Order(OrdCode,0);
					return;
				}
				WatchDog();
				
 #if UPDATA_EXIT
				W25QXX_Write(ADDR_PRO_DOWNLOAD + (RecIngNum - 1) * RecOneLen, &databuf[pos], datalen);
//				W25QXX_Read(ADDR_PRO_DOWNLOAD + (RecIngNum - 1) * RecOneLen, &databuf[pos], datalen);
#else
				iap_write_appbin(ADDR_PRO_DOWNLOAD + (RecIngNum - 1) * RecOneLen,&databuf[pos],datalen);
#endif

				UpdateSuccessAddrFlag = 0x55;
 #if UPDATA_EXIT
				W25QXX_Write(ADDR_UPDATA_SUCCESS_FLAG, &UpdateSuccessAddrFlag, 1);
//				W25QXX_Read1(ADDR_UPDATA_SUCCESS_FLAG, &UpdateSuccessAddrFlag, 1);
#else
				STMFLASH_Write(ADDR_UPDATA_SUCCESS_FLAG, &UpdateSuccessAddrFlag, 1);
//				STMFLASH_Read(ADDR_UPDATA_SUCCESS_FLAG, &UpdateSuccessAddrFlag, 1);
#endif
				Respone_Upper_Order(OrdCode,1);
				Write_Updata_Pro_Data();
				UpdataRestartTimeCs = 0;
//				Soft_Reset();		// 1min后重启
//				iap_load_app(ADDR_IAP_BASE);
//				RCC_DeInit();//关闭外设
//				while(1);
			}
			break;
		default:
//			rsu[0] = COMMU_ERROR_NONSUPORT;			// 不支持指令 
//			return_Comm_Info(OrdFrame, OrdMark, OrdCode, OrdVer, rsu, 1);
			Respone_Upper_Order(OrdCode,0);
			return;
	}
	return;
}

void Respone_Upper_Order(u8 mode,u8 info)
{
	u8 i,SendBuf[20] = {0},DataLen = 0,pos = 0,len,transbuf[5],lastbuf[5],l;
	WatchDog();
	SendBuf[DataLen++] = 0x3c;
	SendBuf[DataLen++] = 0x23;
	pos = DataLen;					
	SendBuf[DataLen++] = 0;		//长度
	SendBuf[DataLen++] = 0;		//长度

	SendBuf[DataLen++] = 8;		//编码长度
	for(i = 0; i < 8; i++)
		SendBuf[DataLen++] = SUPER_STATION_NUMMBER[i];
		
	SendBuf[DataLen++] = 1;		//帧号
	SendBuf[DataLen++] = 2;		//指令方向 
	SendBuf[DataLen++] = 0xA1;	//指令码
	SendBuf[DataLen++] = mode;
	SendBuf[DataLen++] = 2;		//指令版本

	SendBuf[DataLen++] = info;	//应答
	
	// 整理数据长度
	len = DataLen + 3;
	l = change_DEC_to_HEX(len,transbuf);
	char_string_right(transbuf,l,lastbuf,2);
	SendBuf[pos] = lastbuf[0];
	SendBuf[pos+1] = lastbuf[1];

	// CRC
	CRC16(&SendBuf[1],(DataLen - 1));
	SendBuf[DataLen++] = uchCRCLo;
	SendBuf[DataLen++] = uchCRCHi;

	// 结束符
	SendBuf[DataLen++] = ORDER_END_CODE;
UART1_PutString(SendBuf, DataLen);
//#ifdef UART1_PRINT
//	UART1_PutString(SendBuf, DataLen);
//#else
//	UART3_PutString(SendBuf, DataLen);
//#endif
}

void Send_Heart_Pack_Data(void)
{
	WatchDog();
	if(HeartDataTimeCs >= HEARTTIME)
	{
		HeartDataTimeCs = 0;
		Heart_Pack_Data();
	}
}

void Heart_Pack_Data(void)
{
	u8 sendbuf[40]={0},sendlen = 0, i = 0;

	//协议头
	WatchDog();
	
	//水资源协议-----------------
	sendbuf[sendlen++] = WATER_ORDER_START_CODE;					// 水资源帧头
	sendbuf[sendlen++] = 0;				//长度
	sendbuf[sendlen++] = WATER_ORDER_START_CODE;
	sendbuf[sendlen++] = 0x80;			//控制域
	for(i=0;i<5;i++)
		sendbuf[sendlen++] = WaterTerminalAddr[i];		//地址域
	sendbuf[sendlen++] = WATER_ORDER_AFN;
	
	//宏电协议----------------
	sendbuf[sendlen++] = HD_ORDER_START_CODE;			//宏电协议头
	sendbuf[sendlen++] = HD_ORDER_TYPE_CODE;			//宏电协议包类型
	sendbuf[sendlen++] = 0;
	sendbuf[sendlen++] = HD_ORDER_LENGTH_CODE;		//宏电数据长度
	for(i=0;i<11;i++)
		sendbuf[sendlen++] = ComAddr.DTUAddr[i];			//DTU号
	sendbuf[sendlen++] = HD_ORDER_END_CODE;			//宏电协议尾

	sendbuf[1] = sendlen - 3;
	//CS
	sendbuf[sendlen++] = CS_Check(sendbuf, sendlen - 5);
	//协议尾
	sendbuf[sendlen++] = WATER_ORDER_END_CODE;

	UART3_PutString(sendbuf,sendlen);
}


void Read_Com_Addr(void)
{
	u8 i=0,data1=0,data2 = 0;
	u8 transbuf[4],lastbuf[4],datalen;
	u16 PumpNum = 0;
	
	WatchDog();
	W25QXX_Read(ADDR_COM_ADDR, (u8 *)&ComAddr.TerminalAddr[0], sizeof(ComAddr));
	//ASC码转BCD
	for(i=0;i<3;i++)
	{
		data1 = ComAddr.TerminalAddr[i*2];
		data2 = ComAddr.TerminalAddr[i*2+1];
		WaterTerminalAddr[i] = data1<< 4 | data2;
	}

	PumpNum = change_ASC_to_DEC(&ComAddr.TerminalAddr[12], 4);			//井号
	datalen = change_DEC_to_HEX(PumpNum,transbuf);
	char_string_right(transbuf,datalen,lastbuf,2);
	WaterTerminalAddr[3] = lastbuf[0];
	WaterTerminalAddr[4] = lastbuf[1];
}

void Write_Updata_Pro_Data(void)
{
	u16 wbuf[6] = {0},wlen = 0;
	WatchDog();
	wbuf[wlen++] = (u16)(RecTotalLen >> 16);	//高位
	wbuf[wlen++] = (u16)(RecTotalLen & 0xFFFF);	//低位
	wbuf[wlen++] = RecTotalNum;
	wbuf[wlen++] = RecOneLen;
	wbuf[wlen++] = RecLastLen;
	wbuf[wlen++] = RecIngNum;
	STMFLASH_Write(ADDR_REC_TOTAL_LEN, wbuf , wlen);
}

void Read_Updata_Pro_Data(void)
{
	u16 wbuf[6] = {0},wlen = 0,temp1,temp2;
	WatchDog();
	STMFLASH_Read(ADDR_REC_TOTAL_LEN, wbuf , 6);
	temp1	= wbuf[wlen++];		//高位
	temp2	= wbuf[wlen++];		//低位
	RecTotalLen	= temp1 << 16 | temp2;
	
	RecTotalNum	 =wbuf[wlen++];
	RecOneLen	= wbuf[wlen++];
	RecLastLen	= wbuf[wlen++];
	RecIngNum	= wbuf[wlen++];
}

void Reset_Updata_Pro_Data(void)
{
	u16 wbuf[6] = {0};
	WatchDog();
	STMFLASH_Write(ADDR_REC_TOTAL_LEN, wbuf , 6);
}

void Soft_Reset(void) 
{ 
	__set_FAULTMASK(1);      // 关闭所有中端 
	NVIC_SystemReset();// 复位 
} 

void Run_Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

/***********RUN_LED引脚******************/
	GPIO_InitStructure.GPIO_Pin = RUN_LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(RUN_LED_PORT, &GPIO_InitStructure);   
}
void Led_Blink(void)
{
	if(RunLedTimeCs > TIME500MS)
	{
		RunLedTimeCs = 0;
		RUN_LED_OUT = ~RUN_LED_OUT;
	}
}


void NVIC_Config(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

}

void RCC_Clock_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
							 |RCC_APB2Periph_GPIOB
							 |RCC_APB2Periph_GPIOC
							 |RCC_APB2Periph_GPIOD
							 |RCC_APB2Periph_GPIOE
							 ,ENABLE);
}


