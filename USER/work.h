#ifndef __WORK_H
#define __WORK_H
#include "stm32f10x.h"
#include "stmflash.h"

#define  ADDR_COM_ADDR				100				//终端地址DTU地址	
#define HEARTTIME 			TIME1_5MIN

#define PRO_COMPLETE_DATA 0xAA		//验证程序完整

struct ComAddr_define
{
	u8 TerminalAddr[16];				//终端地址(省市县乡村3(BCD)+2bin)
	u8 DTUAddr[11];					//DTU地址
};


//----上位机通讯指令
typedef enum
{
	// ---------------------------------------------------------
	// order mask 
	ORDER_BASE_LENGH 					= 20,	// 命令基本长度

	//水资源协议
	WATER_ORDER_START_CODE			=0x68,	//水资源开始
	WATER_ORDER_AFN					=0xD1,	//水资源AFN
	WATER_ORDER_CONTROL_UP			=0x80,	//控制域上行
	WATER_ORDER_CONTROL_DOWN		=0,		//控制域下行
	WATER_ORDER_END_CODE			=0x16,	//水资源协议结束

	//宏电协议
	HD_ORDER_START_CODE				=0x7B,	//宏电协议起始
	HD_ORDER_TYPE_CODE				=9,		//宏电协议包类型
	HD_ORDER_LENGTH_CODE			=0x10,	//宏电协议长度
	HD_ORDER_END_CODE				=0x7B,	//宏电协议起始

	//德通协议
	ORDER_START_CODE					= 0x3C,	// 命令起始符
	ORDER_VERSION						= 0x23,	// 命令版本号
	ORDER_DATA_VERSION				= 0x22,	// 版本版本(22-带BCD时间   23-无时间)
	ORDER_CODE						= 0xA1,	// 命令标识码

	ORDER_STATION_LENGTH			= 16,		// 终端编码长度

	ORDER_DIR_MD_SEND				= 1,		// 命令方向 - 上位机发起
	ORDER_DIR_TM_RESPOND			= 2,		// 命令方向 - 终端应答
	ORDER_DIR_TM_SEND				= 3,		// 命令方向 - 终端发起
	ORDER_DIR_MD_RESPOND			= 4,		// 命令方向 - 上位机应答

	ORDER_END_CODE					= 0x3e,	// 命令结束符

	// ----------------------------------------------------
	// order code
	ORDER_CODE_WRITE_PARAM					= 0,		// 下发一个参数
	ORDER_CODE_WRITE_PARAM_VER			= 2,		// 命令版本		小数据

	ORDER_CODE_READ_PARAM					= 1,		// 读一个参数
	ORDER_CODE_READ_PARAM_VER				= 2,		// 命令版本

	ORDER_CODE_WRITE_MULTI_PARAM			= 0x06,	// 写多个参数
	ORDER_CODE_WRITE_MULTI_PARAM_VER		= 2,		// 命令版本

	ORDER_CODE_SET_STATE						= 2,		// 设定状态
	ORDER_CODE_SET_STATE_VER				= 2,		// 命令版本


//	ORDER_CODE_ISSUE_CONFIG_FILE			= 0x04,	// 下发配置文件
//	ORDER_CODE_ISSUE_FILE_VER				= 0x03,	// 下发配置文件指令版本

//	ORDER_CODE_UPLOAD_CONFIG_FILE			= 0x05,	// 上载配置文件
//	ORDER_CODE_UPLOAD_FILE_VER				= 0x03,	// 上载配置文件指令版本

	ORDER_CODE_RESET_TERMINAL				=0x02,	//终端复位
	ORDER_CODE_RESET_TERMINAL_VER			=0x02,	//终端复位指令版本

	ORDER_CODE_INQUIRY						= 0x21,	// 召测数据
	ORDER_CODE_INQUIRY_VER					= 0x03,	// 召测数据指令版本

	ORDER_CODE_TIMING						= 0x22,	// 校时
	ORDER_CODE_TIMING_VER					= 0x03,	// 校时指令版本

	ORDER_CODE_INQUIRY_HISTORY				= 0x25,	// 召测历史数据
	ORDER_CODE_INQUIRY_HISTORY_VER			= 0x03,	// 召测历史数据指令版本

	MAIN_CONTER_MARK							= 0x01,	// 主上位系统标识

	ORDER_CODE_TIMED_REPORT				= 0x80,	// 定时上报
	ORDER_CODE_NOTIMED_REPORT_VER			= 0x02,	// 定时上报指令版本    02-应答结果  03-应答结果+上位机ID+时间
	ORDER_CODE_TIMED_REPORT_VER			= 0x03,	// 定时上报指令版本    02-应答结果  03-应答结果+上位机ID+时间
	
	ORDER_CODE_REQUEST_TIMING				= 0x83,	// 终端请求校时
	ORDER_CODE_REQUEST_TIMING_VER			= 0x02,	// 终端请求校时指令版本



	// -----------------------------------------------------
	// 操作设备
	ORDER_WORK_DEVICE_OWN					= 0x00,	// 操作本机

	ORDER_WORK_DEVICE_VALVE					= 0xB1,	// 操作阀门

	ORDER_WORK_TINGZHI						= 0,		// 停止
	ORDER_WORK_QIDONG						= 1,		// 启动
	ORDER_WORK_FUWEI						= 2,		// 复位	
	ORDER_WORK_QIECHU						= 0x10,	// 切除
	ORDER_WORK_TOURU						= 0x11,	// 投入



	ORDER_WORK_PARA_ONOFF					= 0x60,	// 下发启用禁用
	ORDER_WORK_PARA_YALI					= 0x30,	// 下发压力
	ORDER_WORK_PARA_STATION_NUMBER		= 0x10,	// 下发终端编号
	ORDER_WORK_PARA_SHANGBAO_JIANGE		= 0x11,	// 下发上报间隔

	ORDER_WORK_PARA_SYSCFG					= 0x55,	//下发系统参数
	ORDER_WORK_PARA_IRRIGATION_PLAN		= 0x58,	//下发轮灌计划
	

}ORDER_CODE_Info;

// -- 返回操作代码
#define COMMU_RESULT_SUCCEED				1		// 成功
#define COMMU_RESULT_FAIL					0		// 失败
#define COMMU_ERROR_CRC					0x10	// CRC 错
#define COMMU_ERROR_CS					0x12	// CS 错
#define COMMU_ERROR_LENGTH				0x11	// 长度错
#define COMMU_ERROR_NONSUPORT			0x13	// 不支持指令
#define COMMU_ERROR_FORM					0x14	// 参数格式错
#define COMMU_ERROR_SNERR				0x16	// 注册码不匹配
#define COMMU_ERROR_NOFILE				0x17	// 数据不存在


#define RUN_LED_PORT GPIOA
#define RUN_LED_PIN GPIO_Pin_0
#define RUN_LED_OUT PAout(0)

extern u32 UpdataRestartTimeCs;
extern u32 HeartDataTimeCs;


#if UPDATA_EXIT
	extern u8 UpdateSuccessAddrFlag;		//升级成功标志
#else
	extern u16 UpdateSuccessAddrFlag;		//升级成功标志
#endif

void Run_Led_Init(void);
void Led_Blink(void);

void Updata_Program(void);
void Respone_Upper_Order(u8 mode,u8 info);
void Rec_Updata_Data(u8 *buff,u16 length);
void Read_Com_Addr(void);
void Read_Updata_Pro_Data(void);
void Reset_Updata_Pro_Data(void);
void Write_Updata_Pro_Data(void);
void Soft_Reset(void);
void Send_Heart_Pack_Data(void);
void Heart_Pack_Data(void);

void RCC_Clock_Config(void);
void NVIC_Config(void);
#endif
