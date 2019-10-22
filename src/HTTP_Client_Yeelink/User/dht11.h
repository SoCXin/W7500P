#ifndef __DHT11_H
#define __DHT11_H
#include "wizchip_conf.h"
#include "W7500x.h"

//-----------------DHT11���ţ�PA12---------------------
#define	PA2            GPIO_Pin_2
//���κ꣬��������������һ��ʹ��,����ߵ�ƽ��͵�ƽ
#define DHT11_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_2);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_2)
 //��ȡ���ŵĵ�ƽ
#define  DHT11_DATA_IN()	   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)

typedef struct
{
	uint8_t  humi_int;		//ʪ�ȵ���������
	uint8_t  humi_deci;	 	//ʪ�ȵ�С������
	uint8_t  temp_int;	 	//�¶ȵ���������
	uint8_t  temp_deci;	 	//�¶ȵ�С������
	uint8_t  check_sum;	 	//У���
		                 
}DHT11_Data_TypeDef;
extern DHT11_Data_TypeDef DHT11_Data;

void LED_GPIO_Configuration(void);
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);
static uint8_t Read_Byte(void);

#endif
