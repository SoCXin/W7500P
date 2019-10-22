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
#include "hyperterminal.h"
#include "UPnP.h"
#include "MakeXML.h"
#include "loopback.h"
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
					do
					{
						printf("Send SSDP..\r\n");
					}
					while(SSDPProcess(SOCK_SSDP)!=0);
					// Try to Get Description of IGD
					if(GetDescriptionProcess(SOCK_UPNP)==0) 
					printf("GetDescription Success!!\r\n");
					else printf("GetDescription Fail!!\r\n");
					// Try to Subscribe Eventing
					if(SetEventing(SOCK_UPNP)==0) 
					printf("SetEventing Success!!\r\n");
					else 
					printf("SetEventing Fail!!\r\n");
					Main_Menu();
					while(1) {} 
			 }

		 }
}


