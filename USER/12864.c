#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "12864.h"
#include "sys.h"
#include "delay.h"

/*****汉字地址表*****/
u8 addr_tab[]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,//第一行汉字位置
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,//第二行汉字位置
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,//第三行汉字位置
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,//第四行汉字位置
};

char shijian[] = {" 00-00 00:00:00"};

char kaiji[]={
"一二三四五六七八"
"八七六五四三二一"
"二次节电控制终端"
"      V1.0      "
};

char read_err[] =
{
	"参数读取失败"
};

void LCD12864_Init()
{
	GPIO_12864_Init();

	LCD_LED_OUT = 1;//打开背光
	LCD_RST_OUT = 0;
	delay_ms(5);
	LCD_RST_OUT = 1;
	
  	WR_Lcd(CMD,0x30);  /*30---基本指令动作*/ 
	WR_Lcd(CMD,0x03);
	WR_Lcd(CMD,0x0c);  /*开显示，关游标*/
  	WR_Lcd(CMD,0x01);  /*清屏，地址指针指向00H*/
  	WR_Lcd(CMD,0x06);  /*光标的移动方向*/
 	WR_Lcd(CMD,0x30);  /*30---基本指令动作*/  
}


/**********WR_lcd(u8 dat_comm,u8 content)************
*mode: 0为写命令 1为写数据
*w_data:写进的数据
**************************************************/
void WR_Lcd(u8 mode,u8 w_data)
{ 
   
	LCD_CS_OUT = 1;
	//Delayms(10);
	 if(mode==DATA)
 	{
		send_byte(0xFA);//(11111 01 0)先传输五个连续"1",(01)RW=0,RS=1,代表写数据到RAM,显示
	} 
	else 
	{
		send_byte(0xF8);//(11111 00 0)先传输五个连续"1",(00)写命令,控制指令
	}
	//Delayms(5);
	send_byte(0xF0 & w_data);//先写高四位
	//Delayms(5);
	send_byte(0xF0 & (w_data<< 4) );
	//Delayms(2);
	LCD_CS_OUT=0; 
 }

void ClearRAM(void)
{
	WR_Lcd(CMD, 0x30);//进入扩充指令集
	WR_Lcd(CMD, 0x01); //待命模式,进入待命模式，执行任何其他指令都可终止待命模式
}

void send_byte(u8 data)	//【串口时序】
{
	u8 i;	
	for(i=0;i<8;i++)	      //循环n次
	{ 
   		LCD_SCLK_OUT = 0;  
		//Delayus(2);
                
                if(data & 0x80)
                  LCD_RW_OUT = 1;
                else 
                  LCD_RW_OUT = 0;
		//LCD_RW_OUT = ((data & 0x80) >> 8);//出错
		LCD_SCLK_OUT = 1;             //wr电平上跳写入数据wr=1; 
 		delay_us(40);//必须有延时,可以正常显示
		data <<= 1;   	     	//数据左移一位
     }			      
}


void display(unsigned char x_add,char *ptr)
{
  	WR_Lcd(CMD,0x30);//基本指令集

	WR_Lcd(CMD,x_add);// 1       xxx,xxxx 设定DDRAM 7位地址xxx,xxxx到地址计数器AC
	while(*ptr != '\0')
	{
		WR_Lcd(DATA,*ptr);
		++ptr;
	}
}

/*****显示汉字*****/
void Hanzi_Disp(u8 j, u8 k, u8 *s)
{ 
	WR_Lcd(CMD,addr_tab[8*j+k]);  //写地址
	while(*s>0)
	{ 
		WR_Lcd(DATA,*s);    //写数据
		s++;     
	}
}


//显示汉字或字符
void Chn_Disp(unsigned char *chn)
{
  	unsigned char i,j;
  	WR_Lcd(CMD,0x30);//基本指令集
  	//WR_Lcd(CMD,0X01);
  	WR_Lcd(CMD,0x80);//写入地址
  	for(j=0;j<4;j++)
  	{
	    	for(i=0;i<16;i++)
		    	WR_Lcd(DATA,chn[j*16+i]);
  	}
}


//上半屏显示汉字或字符
void Chn_Disp_Half(unsigned char *chn)
{
  	unsigned char i,j;
  	WR_Lcd(CMD,0x30);
  	WR_Lcd(CMD,0x80);
  	j=0;
  	for(i=0;i<16;i++)
	  	WR_Lcd(DATA,chn[j*16+i]);
  	WR_Lcd(CMD,0x90);
  	j=1;
  	for(i=0;i<16;i++)
	  	WR_Lcd(DATA,chn[j*16+i]);
}

//显示图形
void Img_Disp(unsigned char *img)
{
  	u8 i,j;
  	for(j=0;j<32;j++)
  	{
	    	for(i=0;i<8;i++)
	    	{
		      	WR_Lcd(CMD,0x36);
		      	WR_Lcd(CMD,LCD_y+j);
		      	WR_Lcd(CMD,LCD_x1+i);
		      	WR_Lcd(CMD,0x30);
		      	WR_Lcd(DATA,img[j*16+i*2]);
		      	WR_Lcd(DATA,img[j*16+i*2+1]);
	    	}
  	}
  	for(j=32;j<64;j++)
  	{
	    	for(i=0;i<8;i++)
	    	{
		      	WR_Lcd(CMD,0x36);
		      	WR_Lcd(CMD,LCD_y+j-32);
		      	WR_Lcd(CMD,LCD_x2+i);
		      	WR_Lcd(CMD,0x30);
		      	WR_Lcd(DATA,img[j*16+i*2]);
		      	WR_Lcd(DATA,img[j*16+i*2+1]);
	    	}
  	}
  	//WR_Lcd(CMD,0x36);
}

//下半屏显示图形
void Img_Disp_Half(unsigned char *img)
{
  	u8 i,j;
  	for(j=0;j<32;j++)
  	{
	    	for(i=0;i<8;i++)
	    	{
		      	WR_Lcd(CMD,0x34);
		      	WR_Lcd(CMD,LCD_y+j);
		      	WR_Lcd(CMD,LCD_x2+i);
		      	WR_Lcd(CMD,0x30);
		      	WR_Lcd(DATA,img[j*16+i*2]);
		      	WR_Lcd(DATA,img[j*16+i*2+1]);
	    	}
  	}
  	WR_Lcd(CMD,0x36);
}

//显示点阵
void lat_disp(u8 data1,u8 data2)
{
  	u8 i,j,k,x;
  	x=LCD_x1;
  	for(k=0;k<2;k++)
  	{
	    	for(j=0;j<16;j++)
	    	{
		      	for(i=0;i<8;i++)
		      	{
		        	WR_Lcd(CMD,0x36);//GDRAM写数据,先开启扩展指令集（0x36）
				WR_Lcd(CMD,LCD_y+j*2);//写GDRAM地址指令是0x80+地址,先写垂直地址(行地址)
		        	WR_Lcd(CMD,x+i);//写水平地址(列地址)
		        	WR_Lcd(CMD,0x30);
		        	WR_Lcd(DATA,data1);
		        	WR_Lcd(DATA,data1);
		      	}
		      	for(i=0;i<8;i++)
			      	{
			        	WR_Lcd(CMD,0x36);
			        	WR_Lcd(CMD,LCD_y+j*2+1);
			        	WR_Lcd(CMD,x+i);
			        	WR_Lcd(CMD,0x30);
			        	WR_Lcd(DATA,data2);
			        	WR_Lcd(DATA,data2);
			      	}
	    	}
    	x=LCD_x2;
  	}
  	WR_Lcd(CMD,0x30);
}


/*****整屏充满亮点或暗点*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
void Lcd_Fill(u8 test)
{  
	u8 j,k,i;
	for(i=0;i<9;i=i+8)   
	{
		for(j=0;j<0x20;j++)
		{     
			for(k=0;k<8;k++)
			{ 
				WR_Lcd(CMD,0x36);   //扩充指令 绘图显示
				WR_Lcd(CMD,j+0x80);       //行地址
				WR_Lcd(CMD,k+0x80+i);     //列地址     
				WR_Lcd(DATA,test);   //写数据 D15－D8 
				WR_Lcd(DATA,test);   //写数据 D7－D0  
				WR_Lcd(CMD,0x30);
			}
		}
	}
 
}

/*****整屏显示竖条*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
//当i=0时显示上面128×32,当i=8时显示下面128×32
void LcdFill_Vertical()
{  
	u8 i,j,k;
	for(i=0;i<9;i=i+8)   
	{
		for(j=0;j<0x20;j++)
		{     
			for(k=0;k<8;k++)
			   { 
				WR_Lcd(CMD,0x36);   //扩充指令 绘图显示
				WR_Lcd(CMD,j+0x80);       //行地址
				WR_Lcd(CMD,k+0x80+i);     //列地址     
				WR_Lcd(DATA,0x55);   //写数据 D15－D8 
				WR_Lcd(DATA,0x55);   //写数据 D7－D0  
				WR_Lcd(CMD,0x30);
			   }
		} 
	}
}

/*****整屏显示横条*****/
void LcdFill_Level()
{ 
	u8 i,j,k;
	u8 m;   
	for(i=0;i<9;i+=8)    
	{
		for(j=0;j<0x20;j++)
		{     
			if((j%2)==1) m=0xff;
				else m=0;
			for(k=0;k<8;k++)
			{  
				WR_Lcd(CMD,0x36);   //扩充指令 绘图显示
				WR_Lcd(CMD,j+0x80); //行地址
				WR_Lcd(CMD,k+0x80+i); //列地址            
				WR_Lcd(DATA,m);
				WR_Lcd(DATA,m);
				WR_Lcd(CMD,0x30);
			}
		}     
	}
}

/*****显示图片（显示Gdram内容）*****/

/*绘图显示的步骤
1 关闭绘图显示功能
2 先将垂直的坐标(Y)写入绘图RAM地址
3 再将水平的位元组坐标(X)写入绘图RAM地址
4 将D15－D8写入RAM中
5 将D7－D0写入到RAM中
6 打开绘图显示功能*/

//图片取模方式：横向取模，字节正序
void pic_Disp(u8 *pic)    //显示Gdram内容（显示图片）
{
	u8 i,j,k;
	for(i=0;i<9;i=i+8)
	{
		for(j=0;j<32;j++)
		{
			for(k=0;k<8;k++)
			{
				WR_Lcd(CMD,0x36);   //扩充指令，开绘图显示
				WR_Lcd(CMD,0x80+j);  //行地址
				WR_Lcd(CMD,0x80+k+i);  //列地址
				WR_Lcd(DATA,*pic++);  //写数据 D15－D8
				WR_Lcd(DATA,*pic++);  //写数据 D7－D0
				WR_Lcd(CMD,0x30);
			}
		}
	}
}


/*****串行接收一个字节*****/
u8 ReceiveByte(void)
{
     u8 i,temp1,temp2;
     temp1 = 0;
     temp2 = 0;
     for(i=0;i<8;i++)
     {
           temp1=temp1<<1;
           LCD_SCLK_OUT= 0;
           LCD_SCLK_OUT = 1;            
           LCD_SCLK_OUT = 0;
           if(LCD_RW_OUT) temp1++;
     }
     for(i=0;i<8;i++)
     {
           temp2=temp2<<1;
           LCD_SCLK_OUT = 0;
           LCD_SCLK_OUT = 1;
           LCD_SCLK_OUT = 0;
           if(LCD_RW_OUT) temp2++;
     }
     return ((0xf0&temp1)+(0x0f&temp2));
}

/*****检测LCD忙状态*****/
void check_busy( void )
{
     do   send_byte(0xfc);         //11111,RW(1),RS(0),0
     while(0x80&ReceiveByte());       //BF(.7)=1 Busy
}


void GPIO_12864_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//LCD_LED
	GPIO_InitStructure.GPIO_Pin = LCD_LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_LED_PORT, &GPIO_InitStructure);

	//LCD_RST
	GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
	GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);

	//LCD_SCLK
	GPIO_InitStructure.GPIO_Pin = LCD_SCLK_PIN;
	GPIO_Init(LCD_SCLK_PORT, &GPIO_InitStructure);

	//LCD_SW
	GPIO_InitStructure.GPIO_Pin = LCD_RW_PIN;
	GPIO_Init(LCD_RW_PORT, &GPIO_InitStructure);

	//LCD_CS
	GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;
	GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);

	
}
