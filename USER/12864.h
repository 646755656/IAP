#ifndef __12864_H
#define __12864_H


//引脚初始化
//LCD_LED背光
#define LCD_LED_PORT GPIOA
#define LCD_LED_PIN GPIO_Pin_7
#define LCD_LED_OUT PAout(7)

//LCD_RST复位
#define LCD_RST_PORT GPIOC
#define LCD_RST_PIN GPIO_Pin_4
#define LCD_RST_OUT PCout(4)


//LCD_SCLK时钟
#define LCD_SCLK_PORT GPIOC
#define LCD_SCLK_PIN GPIO_Pin_5
#define LCD_SCLK_OUT PCout(5)

//LCD_RW数据
#define LCD_RW_PORT GPIOB
#define LCD_RW_PIN GPIO_Pin_0
#define LCD_RW_OUT PBout(0)

//LCD_CS数据
#define LCD_CS_PORT GPIOB
#define LCD_CS_PIN GPIO_Pin_1
#define LCD_CS_OUT PBout(1)


#define LCD_x1    0x80
#define LCD_x2    0x88
#define LCD_y     0x80
#define CMD  0
#define DATA   1

extern char kaiji[];
extern u8 tab31[];
extern char shijian[];
extern char read_err[];

void LCD12864_Init(void);
void GPIO_12864_Init(void);
void WR_Lcd(u8 mode,u8 w_data);
void send_byte(u8 data);	//【串口时序】
void ClearRAM(void);
void Chn_Disp(unsigned char *chn);
void Chn_Disp_Half(unsigned char *chn);
void Img_Disp(unsigned char *img);
void Img_Disp_Half(unsigned char *img);
void display(unsigned char x_add,char *ptr);

void check_busy(void);
u8 ReceiveByte(void);
void lat_disp(u8 data1,u8 data2);

void Hanzi_Disp(u8 j, u8 k, u8 *s);
void Lcd_Fill(u8 test);
void LcdFill_Vertical();
void LcdFill_Level();
void pic_Disp(u8 *pic);    //显示Gdram内容（显示图片）

#endif
