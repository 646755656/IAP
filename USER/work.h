#ifndef __WORK_H
#define __WORK_H
#include "stm32f10x.h"
#include "stmflash.h"

#define  ADDR_COM_ADDR				100				//�ն˵�ַDTU��ַ	
#define HEARTTIME 			TIME1_5MIN

#define PRO_COMPLETE_DATA 0xAA		//��֤��������

struct ComAddr_define
{
	u8 TerminalAddr[16];				//�ն˵�ַ(ʡ�������3(BCD)+2bin)
	u8 DTUAddr[11];					//DTU��ַ
};


//----��λ��ͨѶָ��
typedef enum
{
	// ---------------------------------------------------------
	// order mask 
	ORDER_BASE_LENGH 					= 20,	// �����������

	//ˮ��ԴЭ��
	WATER_ORDER_START_CODE			=0x68,	//ˮ��Դ��ʼ
	WATER_ORDER_AFN					=0xD1,	//ˮ��ԴAFN
	WATER_ORDER_CONTROL_UP			=0x80,	//����������
	WATER_ORDER_CONTROL_DOWN		=0,		//����������
	WATER_ORDER_END_CODE			=0x16,	//ˮ��ԴЭ�����

	//���Э��
	HD_ORDER_START_CODE				=0x7B,	//���Э����ʼ
	HD_ORDER_TYPE_CODE				=9,		//���Э�������
	HD_ORDER_LENGTH_CODE			=0x10,	//���Э�鳤��
	HD_ORDER_END_CODE				=0x7B,	//���Э����ʼ

	//��ͨЭ��
	ORDER_START_CODE					= 0x3C,	// ������ʼ��
	ORDER_VERSION						= 0x23,	// ����汾��
	ORDER_DATA_VERSION				= 0x22,	// �汾�汾(22-��BCDʱ��   23-��ʱ��)
	ORDER_CODE						= 0xA1,	// �����ʶ��

	ORDER_STATION_LENGTH			= 16,		// �ն˱��볤��

	ORDER_DIR_MD_SEND				= 1,		// ����� - ��λ������
	ORDER_DIR_TM_RESPOND			= 2,		// ����� - �ն�Ӧ��
	ORDER_DIR_TM_SEND				= 3,		// ����� - �ն˷���
	ORDER_DIR_MD_RESPOND			= 4,		// ����� - ��λ��Ӧ��

	ORDER_END_CODE					= 0x3e,	// ���������

	// ----------------------------------------------------
	// order code
	ORDER_CODE_WRITE_PARAM					= 0,		// �·�һ������
	ORDER_CODE_WRITE_PARAM_VER			= 2,		// ����汾		С����

	ORDER_CODE_READ_PARAM					= 1,		// ��һ������
	ORDER_CODE_READ_PARAM_VER				= 2,		// ����汾

	ORDER_CODE_WRITE_MULTI_PARAM			= 0x06,	// д�������
	ORDER_CODE_WRITE_MULTI_PARAM_VER		= 2,		// ����汾

	ORDER_CODE_SET_STATE						= 2,		// �趨״̬
	ORDER_CODE_SET_STATE_VER				= 2,		// ����汾


//	ORDER_CODE_ISSUE_CONFIG_FILE			= 0x04,	// �·������ļ�
//	ORDER_CODE_ISSUE_FILE_VER				= 0x03,	// �·������ļ�ָ��汾

//	ORDER_CODE_UPLOAD_CONFIG_FILE			= 0x05,	// ���������ļ�
//	ORDER_CODE_UPLOAD_FILE_VER				= 0x03,	// ���������ļ�ָ��汾

	ORDER_CODE_RESET_TERMINAL				=0x02,	//�ն˸�λ
	ORDER_CODE_RESET_TERMINAL_VER			=0x02,	//�ն˸�λָ��汾

	ORDER_CODE_INQUIRY						= 0x21,	// �ٲ�����
	ORDER_CODE_INQUIRY_VER					= 0x03,	// �ٲ�����ָ��汾

	ORDER_CODE_TIMING						= 0x22,	// Уʱ
	ORDER_CODE_TIMING_VER					= 0x03,	// Уʱָ��汾

	ORDER_CODE_INQUIRY_HISTORY				= 0x25,	// �ٲ���ʷ����
	ORDER_CODE_INQUIRY_HISTORY_VER			= 0x03,	// �ٲ���ʷ����ָ��汾

	MAIN_CONTER_MARK							= 0x01,	// ����λϵͳ��ʶ

	ORDER_CODE_TIMED_REPORT				= 0x80,	// ��ʱ�ϱ�
	ORDER_CODE_NOTIMED_REPORT_VER			= 0x02,	// ��ʱ�ϱ�ָ��汾    02-Ӧ����  03-Ӧ����+��λ��ID+ʱ��
	ORDER_CODE_TIMED_REPORT_VER			= 0x03,	// ��ʱ�ϱ�ָ��汾    02-Ӧ����  03-Ӧ����+��λ��ID+ʱ��
	
	ORDER_CODE_REQUEST_TIMING				= 0x83,	// �ն�����Уʱ
	ORDER_CODE_REQUEST_TIMING_VER			= 0x02,	// �ն�����Уʱָ��汾



	// -----------------------------------------------------
	// �����豸
	ORDER_WORK_DEVICE_OWN					= 0x00,	// ��������

	ORDER_WORK_DEVICE_VALVE					= 0xB1,	// ��������

	ORDER_WORK_TINGZHI						= 0,		// ֹͣ
	ORDER_WORK_QIDONG						= 1,		// ����
	ORDER_WORK_FUWEI						= 2,		// ��λ	
	ORDER_WORK_QIECHU						= 0x10,	// �г�
	ORDER_WORK_TOURU						= 0x11,	// Ͷ��



	ORDER_WORK_PARA_ONOFF					= 0x60,	// �·����ý���
	ORDER_WORK_PARA_YALI					= 0x30,	// �·�ѹ��
	ORDER_WORK_PARA_STATION_NUMBER		= 0x10,	// �·��ն˱��
	ORDER_WORK_PARA_SHANGBAO_JIANGE		= 0x11,	// �·��ϱ����

	ORDER_WORK_PARA_SYSCFG					= 0x55,	//�·�ϵͳ����
	ORDER_WORK_PARA_IRRIGATION_PLAN		= 0x58,	//�·��ֹ�ƻ�
	

}ORDER_CODE_Info;

// -- ���ز�������
#define COMMU_RESULT_SUCCEED				1		// �ɹ�
#define COMMU_RESULT_FAIL					0		// ʧ��
#define COMMU_ERROR_CRC					0x10	// CRC ��
#define COMMU_ERROR_CS					0x12	// CS ��
#define COMMU_ERROR_LENGTH				0x11	// ���ȴ�
#define COMMU_ERROR_NONSUPORT			0x13	// ��֧��ָ��
#define COMMU_ERROR_FORM					0x14	// ������ʽ��
#define COMMU_ERROR_SNERR				0x16	// ע���벻ƥ��
#define COMMU_ERROR_NOFILE				0x17	// ���ݲ�����


#define RUN_LED_PORT GPIOA
#define RUN_LED_PIN GPIO_Pin_0
#define RUN_LED_OUT PAout(0)

extern u32 UpdataRestartTimeCs;
extern u32 HeartDataTimeCs;


#if UPDATA_EXIT
	extern u8 UpdateSuccessAddrFlag;		//�����ɹ���־
#else
	extern u16 UpdateSuccessAddrFlag;		//�����ɹ���־
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
