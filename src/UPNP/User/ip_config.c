/*
**************************************************************************************************
* @file    		ip_config.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-xx-xx
* @brief  		配置MCU，移植W7500程序需要修改的文件，配置W7500的MAC和IP地址
**************************************************************************************************
*/
#include <string.h>
#include "ip_config.h"
#include "time_delay.h"
#include "socket.h"
#include "dhcp.h"

CONFIG_MSG 	ConfigMsg;
uint32_t my_time;
//uint8_t tx_size[8] = { 2, 2, 2, 2, 2, 2, 2, 2 };
//uint8_t rx_size[8] = { 2, 2, 2, 2, 2, 2, 2, 2 };	
/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x03}; 
uint8_t lip[4] = {192, 168,1,130};									/*定义W5500默认IP地址*/
uint8_t gw[4]  = {192, 168,1,1};										/*定义W5500默认网关*/
uint8_t sub[4] = {255, 255, 255,  0};								/*定义W5500默认子网掩码*/
uint8_t dns_server[4] = {114,114,114,114};   				/*定义W5500默认DNS*/
uint16_t local_port=5000;														/*定义本地端口*/

uint8_t	remote_ip[4]={192, 168,0,110};							/*远端IP地址*/
uint16_t remote_port=6000;													/*远端端口号*/

uint8_t	ip_from_dhcp=1;
uint8_t ip_from_define=0;	
/**
*@brief		配置W7500MAC地址
*@param		无
*@return	无
*/
void set_W7500_mac(void)
{
		memcpy(ConfigMsg.mac,mac,4);
	  setSHAR(mac);
}
/**
*@brief		配置W7500 IP地址
*@param		无
*@return	无
*/
void set_w7500_ip(void)
{
    /* Network Configuration */
		if(ip_from_define==1)
		{
			printf(" IP from define	\r\n");	
			memcpy(ConfigMsg.lip, lip, 4);
			memcpy(ConfigMsg.sub, sub, 4);
			memcpy(ConfigMsg.gw,  gw, 4);
			memcpy(ConfigMsg.dns,dns_server,4);
		}
		if(ip_from_dhcp==1)								
		{
			memcpy(ConfigMsg.lip, DHCP_allocated_ip, 4);
			memcpy(ConfigMsg.sub, DHCP_allocated_sn, 4);
			memcpy(ConfigMsg.gw,  DHCP_allocated_gw, 4);
			memcpy(ConfigMsg.dns, DHCP_allocated_dns,4);			
			
		}
			setSIPR(ConfigMsg.lip);
			setGAR(ConfigMsg.gw);
			setSUBR(ConfigMsg.sub);
    /* Set Network Configuration */
    //wizchip_init(tx_size, rx_size);
   // getSHAR(mac);
    printf(" MAC ADDRESS : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]); 
    getSIPR(lip);
    printf("IP ADDRESS : %d.%d.%d.%d\r\n",lip[0],lip[1],lip[2],lip[3]); 
    getGAR(gw);
    printf("GW ADDRESS : %d.%d.%d.%d\r\n",gw[0],gw[1],gw[2],gw[3]); 
    getSUBR(sub);
    printf("SN MASK: %d.%d.%d.%d\r\n",sub[0],sub[1],sub[2],sub[3]); 
}
/**
*@brief		PHY初始化配置
*@param		无
*@return	无
*/
void phy_init(void)
{
    setTIC100US((GetSystemClock()/10000));    		/* 设置 WZ_100US寄存器*/
		#ifdef __DEF_USED_IC101AG__ //For using IC+101AG
		*(volatile uint32_t *)(0x41003068) = 0x64; //TXD0 - set PAD strengh and pull-up
		*(volatile uint32_t *)(0x4100306C) = 0x64; //TXD1 - set PAD strengh and pull-up
		*(volatile uint32_t *)(0x41003070) = 0x64; //TXD2 - set PAD strengh and pull-up
		*(volatile uint32_t *)(0x41003074) = 0x64; //TXD3 - set PAD strengh and pull-up
		*(volatile uint32_t *)(0x41003050) = 0x64; //TXE  - set PAD strengh and pull-up
		#endif	
		#ifdef __W7500P__
		*(volatile uint32_t *)(0x41003070) = 0x61;
		*(volatile uint32_t *)(0x41003054) = 0x61;
		#endif	
		#ifdef __DEF_USED_MDIO__ 
    mdio_init(GPIOB, MDC, MDIO );    							/* mdio接口初始化 */
    while( link() == 0x0 )   				 							/* 判断PHY 连接是否成功 */
    {
        printf(".");  
        delay_ms(500);
    }
    printf("PHY is linked. \r\n");  
		#endif
}

