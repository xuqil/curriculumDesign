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
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
 	LCD_Init();
	KEY_Init();				//������ʼ��		 	
	SPI_Flash_Init();  		//SPI FLASH ��ʼ�� 	 
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	while(SPI_Flash_ReadID()!=W25Q64)							//��ⲻ��W25Q64
	{
		LCD_ShowString(60,190,200,16,16,"25Q64 Check Failed!");
		delay_ms(500);
		LCD_ShowString(60,190,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}	
	SPI_Flash_Read(datatemp,size-100,size);	
	LCD_ShowString(60,190,200,16,16,"25Q64 Ready!");

	FLASH_SIZE=8*1024*1024;	//FLASH ��СΪ8M�ֽ�
  POINT_COLOR=BLUE;		//��������Ϊ��ɫ	 

 	
 	while(DS18B20_Init())	//DS18B20��ʼ��	
	{
		LCD_ShowString(60,130,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,130,200,16,16,"DS18B20 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
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
		if(key==WKUP_PRES)	//WK_UP ����,д��W25Q64
		{
			LCD_Fill(0,220,239,319,WHITE);//�������    
 			LCD_ShowString(60,240,200,16,16,"Start Write W25Q64....");
			SPI_Flash_Write((u8*)buffer,size-100,size);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
			LCD_ShowString(60,240,200,16,16,"W25Q64 Write Finished!");	//��ʾ�������
		}
		if(key==KEY0_PRES)	//KEY0 ����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(60,220,200,16,16,"Start Read W25Q64.... ");
			SPI_Flash_Read(datatemp,size-100,size);				//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
			LCD_ShowString(60,240,200,16,16,"The Data Readed Is:  ");	//��ʾ�������
			LCD_ShowString(60,260,200,16,16,datatemp);					//��ʾ�������ַ���
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
 		if(t%10==0)//ÿ100ms��ȡһ��
		{									  
			temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(60+40,150,'-',16,0);			//��ʾ����
				temperature=-temperature;					//תΪ����
			}else LCD_ShowChar(60+40,150,' ',16,0);			//ȥ������
			LCD_ShowNum(60+40+8,150,temperature/10,2,16);	//��ʾ��������	    
   			LCD_ShowNum(60+40+32,150,temperature%10,1,16);	//��ʾС������ 		   
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
