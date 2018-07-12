#ifndef __UART_H
#define __UART_H

#define TX1_PORT GPIOA
#define TX1_PIN GPIO_Pin_9

#define RX1_PORT GPIOA
#define RX1_PIN GPIO_Pin_10

#define TX2_PORT GPIOA
#define TX2_PIN GPIO_Pin_2

#define RX2_PORT GPIOA
#define RX2_PIN GPIO_Pin_3

#define TX3_PORT GPIOB
#define TX3_PIN GPIO_Pin_10

#define RX3_PORT GPIOB
#define RX3_PIN GPIO_Pin_11

#define TX4_PORT GPIOC
#define TX4_PIN GPIO_Pin_10

#define RX4_PORT GPIOC
#define RX4_PIN GPIO_Pin_11

#define TX5_PORT GPIOC
#define TX5_PIN GPIO_Pin_12

#define RX5_PORT GPIOD
#define RX5_PIN GPIO_Pin_2


#define RCV1_BUFF_LENGH 1250
#define RCV2_BUFF_LENGH 100
#define RCV3_BUFF_LENGH 400
#define RCV4_BUFF_LENGH 150
#define RCV5_BUFF_LENGH 200
#define OK 1
#define NO 0


struct com1_define{
	unsigned int rcv_len;
	unsigned char rcv_buff[RCV1_BUFF_LENGH];
	unsigned int send_len;
	unsigned char rcv_flag;
	unsigned char jg_time;
};


struct com2_define{
	unsigned int rcv_len;
	unsigned char rcv_buff[RCV2_BUFF_LENGH];
	unsigned int send_len;
	unsigned char rcv_flag;
	unsigned char jg_time;
};

struct com3_define{
	unsigned int rcv_len;
	unsigned char rcv_buff[RCV3_BUFF_LENGH];
	unsigned int send_len;
	unsigned char rcv_flag;
	unsigned char jg_time;
};
struct com4_define{
	unsigned int rcv_len;
	unsigned char rcv_buff[RCV4_BUFF_LENGH];
	unsigned int send_len;
	unsigned char rcv_flag;
	unsigned char jg_time;
};
struct com5_define{
	unsigned int rcv_len;
	unsigned char rcv_buff[RCV5_BUFF_LENGH];
	unsigned int send_len;
	unsigned char rcv_flag;
	unsigned char jg_time;
};

extern struct com1_define COM1;
extern struct com2_define COM2;
extern struct com3_define COM3;
extern struct com4_define COM4;
extern struct com5_define COM5;

void UART_Init(void);
void USART1_Init(unsigned int baut);
void USART2_Init(u32 baut);
void USART3_Init(unsigned int baut);
void UART4_Init(unsigned int baut);
void UART5_Init(unsigned int baut);

void UART1_PutChar(unsigned char ch);
void UART1_PutString(unsigned char *buf, unsigned int len);
void UART1_Clear(void);
void UART2_PutChar(unsigned char ch);
void UART2_PutString(unsigned char *buf, unsigned int len);
void UART2_Clear(void);
void UART3_PutChar(unsigned char ch);
void UART3_PutString(unsigned char *buf, unsigned int len);
void UART3_Clear(void);
void UART4_PutChar(unsigned char ch);
void UART4_PutString(unsigned char *buf, unsigned int len);
void UART4_Clear(void);
void UART5_PutChar(unsigned char ch);
void UART5_PutString(unsigned char *buf, unsigned int len);
void UART5_Clear(void);

#endif
