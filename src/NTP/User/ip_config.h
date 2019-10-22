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

extern uint8_t	remote_ip[4];
extern uint16_t	local_port;
extern uint16_t	remote_port;

#define SOCK_NTP              0
#define TX_RX_MAX_BUF_SIZE    2048	
#pragma pack(1)
typedef struct _DateTime
{
  uint16_t year[2];
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} Date;

typedef struct _CONFIG_MSG
{
  uint8_t op[4];//header: FIND;SETT;FACT...
  uint8_t mac[6];
  uint8_t sw_ver[2];
  uint8_t lip[4];
  uint8_t sub[4];
  uint8_t gw[4];
  uint8_t dns[4];	
  uint8_t dhcp;
  uint8_t debug;

  uint16_t fw_len;
  uint8_t  state;
  
    Date date;
}CONFIG_MSG;
#pragma pack()
extern CONFIG_MSG  ConfigMsg, RecvMsg;

void set_w7500_ip(void);//����w7500 IP��ַ��Ϣ
void phy_init(void);		//PHY��ʼ��
#endif

