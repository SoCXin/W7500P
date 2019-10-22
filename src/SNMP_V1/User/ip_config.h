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

#define SOCK_TCPS	0
#define SOCK_DHCP	2

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

void set_W7500_mac(void);
void set_w7500_ip(void);//设置w7500 IP地址信息
void phy_init(void);		//PHY初始化
#endif

