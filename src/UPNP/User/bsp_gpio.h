#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_
#include "wizchip_conf.h"
#include "W7500x.h"
void LED_GPIO_Configuration(void);  	//LED 相关引脚初始化
void blink_led(void);				//RGB交互循环点亮
void blink1_led(void);
void redled_toggle(void);		//红色LED反转
#endif
