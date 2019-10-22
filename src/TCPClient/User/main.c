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
#include "tcpc.h"

uint8_t test_buf[2048];													/*����һ��2KB�Ļ���*/

int main()
{
    SystemInit();     												 		/* ϵͳ�ڲ�ʱ�ӳ�ʼ�� */ 
		delay_init();																	/* ϵͳ�δ�ʱ������*/ 
//		UART_Configuration();													/*��ʼ������ͨ��:115200@8-n-1*/	   
		S_UART_Init(115200);													 /* Configure UART2 */
		phy_init();																		/*PHY��ʼ������ */
		set_w7500_ip();
		printf(" ������ΪTCP������,��W5500��Ϊ TCP�ͻ���ȥ���� \r\n");
		printf(" PC������IPĬ��Ϊ:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
		printf(" PC�������˿�Ĭ��Ϊ:%d \r\n",remote_port);
		printf(" ʵ���������ӳɹ���PC���������ݸ�W5500��W5500�����ض�Ӧ����\r\n"); 
    while(1)
    {
			tcpc(0,test_buf,remote_ip,remote_port,local_port);							/*TCP_Server ���ݻػ����Գ���*/
    }
}


