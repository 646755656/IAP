#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "12864.h"
#include "sys.h"
#include "delay.h"

/*****���ֵ�ַ��*****/
u8 addr_tab[]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,//��һ�к���λ��
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,//�ڶ��к���λ��
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,//�����к���λ��
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,//�����к���λ��
};

char shijian[] = {" 00-00 00:00:00"};

char kaiji[]={
"һ�����������߰�"
"��������������һ"
"���νڵ�����ն�"
"      V1.0      "
};

char read_err[] =
{
	"������ȡʧ��"
};

void LCD12864_Init()
{
	GPIO_12864_Init();

	LCD_LED_OUT = 1;//�򿪱���
	LCD_RST_OUT = 0;
	delay_ms(5);
	LCD_RST_OUT = 1;
	
  	WR_Lcd(CMD,0x30);  /*30---����ָ���*/ 
	WR_Lcd(CMD,0x03);
	WR_Lcd(CMD,0x0c);  /*����ʾ�����α�*/
  	WR_Lcd(CMD,0x01);  /*��������ַָ��ָ��00H*/
  	WR_Lcd(CMD,0x06);  /*�����ƶ�����*/
 	WR_Lcd(CMD,0x30);  /*30---����ָ���*/  
}


/**********WR_lcd(u8 dat_comm,u8 content)************
*mode: 0Ϊд���� 1Ϊд����
*w_data:д��������
**************************************************/
void WR_Lcd(u8 mode,u8 w_data)
{ 
   
	LCD_CS_OUT = 1;
	//Delayms(10);
	 if(mode==DATA)
 	{
		send_byte(0xFA);//(11111 01 0)�ȴ����������"1",(01)RW=0,RS=1,����д���ݵ�RAM,��ʾ
	} 
	else 
	{
		send_byte(0xF8);//(11111 00 0)�ȴ����������"1",(00)д����,����ָ��
	}
	//Delayms(5);
	send_byte(0xF0 & w_data);//��д����λ
	//Delayms(5);
	send_byte(0xF0 & (w_data<< 4) );
	//Delayms(2);
	LCD_CS_OUT=0; 
 }

void ClearRAM(void)
{
	WR_Lcd(CMD, 0x30);//��������ָ�
	WR_Lcd(CMD, 0x01); //����ģʽ,�������ģʽ��ִ���κ�����ָ�����ֹ����ģʽ
}

void send_byte(u8 data)	//������ʱ��
{
	u8 i;	
	for(i=0;i<8;i++)	      //ѭ��n��
	{ 
   		LCD_SCLK_OUT = 0;  
		//Delayus(2);
                
                if(data & 0x80)
                  LCD_RW_OUT = 1;
                else 
                  LCD_RW_OUT = 0;
		//LCD_RW_OUT = ((data & 0x80) >> 8);//����
		LCD_SCLK_OUT = 1;             //wr��ƽ����д������wr=1; 
 		delay_us(40);//��������ʱ,����������ʾ
		data <<= 1;   	     	//��������һλ
     }			      
}


void display(unsigned char x_add,char *ptr)
{
  	WR_Lcd(CMD,0x30);//����ָ�

	WR_Lcd(CMD,x_add);// 1       xxx,xxxx �趨DDRAM 7λ��ַxxx,xxxx����ַ������AC
	while(*ptr != '\0')
	{
		WR_Lcd(DATA,*ptr);
		++ptr;
	}
}

/*****��ʾ����*****/
void Hanzi_Disp(u8 j, u8 k, u8 *s)
{ 
	WR_Lcd(CMD,addr_tab[8*j+k]);  //д��ַ
	while(*s>0)
	{ 
		WR_Lcd(DATA,*s);    //д����
		s++;     
	}
}


//��ʾ���ֻ��ַ�
void Chn_Disp(unsigned char *chn)
{
  	unsigned char i,j;
  	WR_Lcd(CMD,0x30);//����ָ�
  	//WR_Lcd(CMD,0X01);
  	WR_Lcd(CMD,0x80);//д���ַ
  	for(j=0;j<4;j++)
  	{
	    	for(i=0;i<16;i++)
		    	WR_Lcd(DATA,chn[j*16+i]);
  	}
}


//�ϰ�����ʾ���ֻ��ַ�
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

//��ʾͼ��
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

//�°�����ʾͼ��
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

//��ʾ����
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
		        	WR_Lcd(CMD,0x36);//GDRAMд����,�ȿ�����չָ���0x36��
				WR_Lcd(CMD,LCD_y+j*2);//дGDRAM��ַָ����0x80+��ַ,��д��ֱ��ַ(�е�ַ)
		        	WR_Lcd(CMD,x+i);//дˮƽ��ַ(�е�ַ)
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


/*****������������򰵵�*****/
//��i=0ʱ��ʾ����128��32,��i=8ʱ��ʾ����128��32
void Lcd_Fill(u8 test)
{  
	u8 j,k,i;
	for(i=0;i<9;i=i+8)   
	{
		for(j=0;j<0x20;j++)
		{     
			for(k=0;k<8;k++)
			{ 
				WR_Lcd(CMD,0x36);   //����ָ�� ��ͼ��ʾ
				WR_Lcd(CMD,j+0x80);       //�е�ַ
				WR_Lcd(CMD,k+0x80+i);     //�е�ַ     
				WR_Lcd(DATA,test);   //д���� D15��D8 
				WR_Lcd(DATA,test);   //д���� D7��D0  
				WR_Lcd(CMD,0x30);
			}
		}
	}
 
}

/*****������ʾ����*****/
//��i=0ʱ��ʾ����128��32,��i=8ʱ��ʾ����128��32
//��i=0ʱ��ʾ����128��32,��i=8ʱ��ʾ����128��32
void LcdFill_Vertical()
{  
	u8 i,j,k;
	for(i=0;i<9;i=i+8)   
	{
		for(j=0;j<0x20;j++)
		{     
			for(k=0;k<8;k++)
			   { 
				WR_Lcd(CMD,0x36);   //����ָ�� ��ͼ��ʾ
				WR_Lcd(CMD,j+0x80);       //�е�ַ
				WR_Lcd(CMD,k+0x80+i);     //�е�ַ     
				WR_Lcd(DATA,0x55);   //д���� D15��D8 
				WR_Lcd(DATA,0x55);   //д���� D7��D0  
				WR_Lcd(CMD,0x30);
			   }
		} 
	}
}

/*****������ʾ����*****/
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
				WR_Lcd(CMD,0x36);   //����ָ�� ��ͼ��ʾ
				WR_Lcd(CMD,j+0x80); //�е�ַ
				WR_Lcd(CMD,k+0x80+i); //�е�ַ            
				WR_Lcd(DATA,m);
				WR_Lcd(DATA,m);
				WR_Lcd(CMD,0x30);
			}
		}     
	}
}

/*****��ʾͼƬ����ʾGdram���ݣ�*****/

/*��ͼ��ʾ�Ĳ���
1 �رջ�ͼ��ʾ����
2 �Ƚ���ֱ������(Y)д���ͼRAM��ַ
3 �ٽ�ˮƽ��λԪ������(X)д���ͼRAM��ַ
4 ��D15��D8д��RAM��
5 ��D7��D0д�뵽RAM��
6 �򿪻�ͼ��ʾ����*/

//ͼƬȡģ��ʽ������ȡģ���ֽ�����
void pic_Disp(u8 *pic)    //��ʾGdram���ݣ���ʾͼƬ��
{
	u8 i,j,k;
	for(i=0;i<9;i=i+8)
	{
		for(j=0;j<32;j++)
		{
			for(k=0;k<8;k++)
			{
				WR_Lcd(CMD,0x36);   //����ָ�����ͼ��ʾ
				WR_Lcd(CMD,0x80+j);  //�е�ַ
				WR_Lcd(CMD,0x80+k+i);  //�е�ַ
				WR_Lcd(DATA,*pic++);  //д���� D15��D8
				WR_Lcd(DATA,*pic++);  //д���� D7��D0
				WR_Lcd(CMD,0x30);
			}
		}
	}
}


/*****���н���һ���ֽ�*****/
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

/*****���LCDæ״̬*****/
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
