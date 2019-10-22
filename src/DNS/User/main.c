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
#include "dns.h"
uint8_t Domain_name[]="www.baidu.com";    // for example domain name
//uint8_t Domain_name[32];    // for example domain name
uint8_t test_buf[2048];
uint8_t Domain_IP[4];           // for Example domain name server

int main()
{
    SystemInit();     												 		/* ϵͳ�ڲ�ʱ�ӳ�ʼ�� */ 
		delay_init();																	/* ϵͳ�δ�ʱ������*/ 
//		UART_Configuration();													/*��ʼ������ͨ��:115200@8-n-1*/	   
		S_UART_Init(115200);													 /* Configure UART2 */
		phy_init();																		/*PHY��ʼ������ */
		set_w7500_ip();
		DNS_init(SOCK_DNS, test_buf);
    printf(" TEST- START \r\n");  
    while(1)
    {
		//	printf("\r\nInput the domain name (ex>www.wiznet.co.kr):\r\n");
		//	scanf("%s",Domain_name);
			printf("\r\n[ %s ]'s IP address is:\r\n",Domain_name);			
			if(DNS_run(dns_server, Domain_name,Domain_IP))
			{
				printf("%d.%d.%d.%d\r\n",Domain_IP[0],Domain_IP[1],Domain_IP[2],Domain_IP[3]);
				break;
			}
			else
			{
				printf("\r\n> DNS Failed\r\n");
			}
			
    }
}


