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
#include "tcpc.h"

uint8_t test_buf[2048];													/*定义一个2KB的缓存*/

int main()
{
    SystemInit();     												 		/* 系统内部时钟初始化 */ 
		delay_init();																	/* 系统滴答时钟配置*/ 
//		UART_Configuration();													/*初始化串口通信:115200@8-n-1*/	   
		S_UART_Init(115200);													 /* Configure UART2 */
		phy_init();																		/*PHY初始化设置 */
		set_w7500_ip();
		printf(" 电脑作为TCP服务器,让W5500作为 TCP客户端去连接 \r\n");
		printf(" PC服务器IP默认为:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
		printf(" PC服务器端口默认为:%d \r\n",remote_port);
		printf(" 实现现象：连接成功后，PC机发送数据给W5500，W5500将返回对应数据\r\n"); 
    while(1)
    {
			tcpc(0,test_buf,remote_ip,remote_port,local_port);							/*TCP_Server 数据回环测试程序*/
    }
}


