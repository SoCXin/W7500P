/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-xx-xx
* @brief   			��iolibary�汾�⽨�Ĺ���ģ��
* @attention  	ʵ��ƽ̨:WIZnet_W7500_wizwiki���������
*
*								������Ϊ�������ԣ��뱣֤W7500�����߿��Է���Internet
******************************************************************************
*/ 
#include <stdio.h>
#include "W7500x.h"
#include "time_delay.h"
#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "ip_config.h"
#include "dhcp.h"
#include "snmpLib.h"
#include "snmpDemo.h"
uint8_t test_buf[2048];													/*����һ��2KB�Ļ���*/

int main()
{
    SystemInit();     												 		/* ϵͳ�ڲ�ʱ�ӳ�ʼ�� */ 
		delay_init();																	/* ϵͳ�δ�ʱ������*/ 
//		UART_Configuration();													/*��ʼ������ͨ��:115200@8-n-1*/	   
		S_UART_Init(115200);													 /* Configure UART2 */
		phy_init();																		/*PHY��ʼ������ */
		set_W7500_mac();
		set_w7500_ip();
		led_gpio_init();
    while(1)
    {
			UserSnmpDemo();	
		}
}


