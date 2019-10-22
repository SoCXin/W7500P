/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-xx-xx
* @brief   			用iolibary版本库建的工程模板
* @attention  	实验平台:WIZnet_W7500_wizwiki网络适配板
*
*								本测试为外网测试，请保证W7500的网线可以访问Internet
******************************************************************************
*/ 
#include <stdio.h>
#include "W7500x.h"
#include "ip_config.h"
#include "time_delay.h"
#include "bsp_uart.h"
#include "dhcp.h"

uint8_t test_buf[2048];													/*定义一个2KB的缓存*/

int main()
{
    SystemInit();     												 		/* 系统内部时钟初始化 */ 
		delay_init();																	/* 系统滴答时钟配置*/ 
//		UART_Configuration();													/*初始化串口通信:115200@8-n-1*/	   
		S_UART_Init(115200);													 /* Configure UART2 */
		phy_init();																		/*PHY初始化设置 */
		set_W7500_mac();
    DHCP_init(SOCK_DHCP, test_buf);								/*DHCP初始化设置 */
    while(1)
    {
			do_dhcp();
       if(toggle == 0)
			 {
					loopback_tcps(0, test_buf, 5000);
			 }
		 }
}


