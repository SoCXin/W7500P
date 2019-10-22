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
#include "dns.h"
#include "httpc.h"
#include "dht11.h"

#define yeelink_id       "gaoyongbiao"        //your sina weibo ID
#define yeelink_pwd      "gaoyongbiao123"        //your sina weibo password
uint8_t test_buf[2048];													/*定义一个2KB的缓存*/

int main()
{
    SystemInit();     												 		/* 系统内部时钟初始化 */ 
		delay_init();																	/* 系统滴答时钟配置*/ 
//		UART_Configuration();													/*初始化串口通信:115200@8-n-1*/	   
		S_UART_Init(115200);													 /* Configure UART2 */
		LED_GPIO_Configuration();	
		phy_init();																		/*PHY初始化设置 */
		set_w7500_ip();
	DNS_init(test_buf);	//这个有很大影响，具体原因还不明确
	printf(" 我的Yelink ID为:%s \r\n",yeelink_id);
	printf(" 我的Yelink PWD为:%s \r\n",yeelink_pwd);
	printf(" 请登录以上账号观察实时温湿度变化	\r\n");
    while(1)
    {	
			  delay_ms(100);
				httpclient(SOCK_HUMTEM,3000);	      		
		}
}


