#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_
#include "wizchip_conf.h"
#include "W7500x.h"
void LED_GPIO_Configuration(void);  	//LED ������ų�ʼ��
void blink_led(void);				//RGB����ѭ������
void blink1_led(void);
void redled_toggle(void);		//��ɫLED��ת
#endif
