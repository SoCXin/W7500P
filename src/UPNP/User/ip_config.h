#ifndef _IP_CONFIG_H_
#define _IP_CONFIG_H_

#include "wizchip_conf.h"
#include "W7500x.h"
#include <stdint.h>
#include <stdio.h>

#define __DEF_USED_MDIO__ 
#define __W7500P__ // for W7500P

#ifndef __W7500P__ // for W7500
	#define __DEF_USED_IC101AG__ //for W7500 Test main Board V001
#endif

#define UDP_LISTEN_PORT					5000
#define SOCK_SSDP								0	// UDP
#define SOCK_UPNP								1	// TCP
#define SOCK_UPNP_EVENTING			2	// TCP
#define SOCK_CONFIG							2	// UDP
#define SOCK_DNS								2	// UDP
#define SOCK_DHCP								3	// UDP
#define SOCK_TCPS								4
extern uint32_t my_time;
extern uint8_t	remote_ip[4];
extern uint16_t	local_port;
extern uint16_t	remote_port;

#pragma pack(1)
typedef struct 
{
	uint8_t mac[6]; 
	uint8_t lip[4];
	uint8_t gw[4];
	uint8_t sub[4];		
	uint8_t dns[4];           // for Example domain name server
} CONFIG_MSG;
#pragma pack()
extern CONFIG_MSG 	ConfigMsg; 
typedef struct _CONFIG_TYPE_DEF
{
	uint16_t port;
	uint8_t destip[4];
}CHCONFIG_TYPE_DEF;
void set_W7500_mac(void);
void set_w7500_ip(void);//设置w7500 IP地址信息
void phy_init(void);		//PHY初始化
#endif

