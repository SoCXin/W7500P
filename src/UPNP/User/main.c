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
#include "ip_config.h"
#include "time_delay.h"
#include "bsp_uart.h"
#include "dhcp.h"
#include "hyperterminal.h"
#include "UPnP.h"
#include "MakeXML.h"
#include "loopback.h"
uint8_t test_buf[2048];													/*����һ��2KB�Ļ���*/

int main()
{
    SystemInit();     												 		/* ϵͳ�ڲ�ʱ�ӳ�ʼ�� */ 
		delay_init();																	/* ϵͳ�δ�ʱ������*/ 
//		UART_Configuration();													/*��ʼ������ͨ��:115200@8-n-1*/	   
		S_UART_Init(115200);													 /* Configure UART2 */
		phy_init();																		/*PHY��ʼ������ */
		set_W7500_mac();
    DHCP_init(SOCK_DHCP, test_buf);								/*DHCP��ʼ������ */
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


