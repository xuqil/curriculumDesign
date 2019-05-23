#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "ds18b20.h"   
#include "key.h"
#include "spi.h"
#include "flash.h"
#include <stdlib.h>

 int main(void)
 { 
	u8 key;
	int value = 20;
	int size = sizeof(value);
	u8 datatemp[10];
	u32 FLASH_SIZE;
	u8 t=0;
  int result;	
	char buffer[20];
	short temperature;   
	delay_init();	    	 //延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
 	LCD_Init();
	KEY_Init();				//按键初始化		 	
	SPI_Flash_Init();  		//SPI FLASH 初始化 	 
 	POINT_COLOR=RED;//设置字体为红色 
	while(SPI_Flash_ReadID()!=W25Q64)							//检测不到W25Q64
	{
		LCD_ShowString(60,190,200,16,16,"25Q64 Check Failed!");
		delay_ms(500);
		LCD_ShowString(60,190,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}	
	SPI_Flash_Read(datatemp,size-100,size);	
	LCD_ShowString(60,190,200,16,16,"25Q64 Ready!");

	FLASH_SIZE=8*1024*1024;	//FLASH 大小为8M字节
  POINT_COLOR=BLUE;		//设置字体为蓝色	 

 	
 	while(DS18B20_Init())	//DS18B20初始化	
	{
		LCD_ShowString(60,130,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,130,200,16,16,"DS18B20 OK");
	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(60,150,200,16,16,"Temp:   . C");	 
	while(1)
	{	  
		sprintf(buffer,"%d",value); 
		key=KEY_Scan(0);
		
		if(key==KEY1_PRES)
		{
			value ++;
			size = sizeof(value);
			sprintf(buffer,"%d",value); 
		}
		LCD_ShowxNum(60,290,value,16,16,0);
		if(key==WKUP_PRES)	//WK_UP 按下,写入W25Q64
		{
			LCD_Fill(0,220,239,319,WHITE);//清除半屏    
 			LCD_ShowString(60,240,200,16,16,"Start Write W25Q64....");
			SPI_Flash_Write((u8*)buffer,size-100,size);		//从倒数第100个地址处开始,写入SIZE长度的数据
			LCD_ShowString(60,240,200,16,16,"W25Q64 Write Finished!");	//提示传送完成
		}
		if(key==KEY0_PRES)	//KEY0 按下,读取字符串并显示
		{
 			LCD_ShowString(60,220,200,16,16,"Start Read W25Q64.... ");
			SPI_Flash_Read(datatemp,size-100,size);				//从倒数第100个地址处开始,读出SIZE个字节
			LCD_ShowString(60,240,200,16,16,"The Data Readed Is:  ");	//提示传送完成
			LCD_ShowString(60,260,200,16,16,datatemp);					//显示读到的字符串
		}
		//printf("datatemp:  %s\n\n",datatemp);
		result = atoi((char*)datatemp);
		if (temperature/10 >= result)
		{
			LED1=!LED1;
			printf("result:  %d\n\n",result);
			printf("temperature:  %d\n\n",temperature/10);
			delay_ms(100);
		}
 		if(t%10==0)//每100ms读取一次
		{									  
			temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(60+40,150,'-',16,0);			//显示负号
				temperature=-temperature;					//转为正数
			}else LCD_ShowChar(60+40,150,' ',16,0);			//去掉负号
			LCD_ShowNum(60+40+8,150,temperature/10,2,16);	//显示正数部分	    
   			LCD_ShowNum(60+40+32,150,temperature%10,1,16);	//显示小数部分 		   
		}	
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0=!LED0;
		}
	}
}
