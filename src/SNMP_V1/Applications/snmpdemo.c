/*
**************************************************************************************************
* @file    		snmpdemo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-xx-xx
* @brief  		
**************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

//#ifdef WIN32
//#include <winsock2.h>
//#else

#include "socket.h"
#include "netutil.h"

//#endif
#include "snmpLib.h"
#include "snmpDemo.h"
#include "time.h"
time_t startTime;
time_t time(time_t *time);
dataEntryType snmpData[] =
{
	// System MIB

	// SysDescr Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 1, 0}, 
	SNMPDTYPE_OCTET_STRING, 30, {"WIZnet Embedded SNMP Agent"}, 
	NULL, NULL},

	// SysObjectID Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 2, 0}, 
	SNMPDTYPE_OBJ_ID, 8, {"\x2b\x06\x01\x02\x01\x01\x02\x00"},
	NULL, NULL},

//	// SysUptime Entry
//	{8, {0x2b, 6, 1, 2, 1, 1, 3, 0}, 
//	SNMPDTYPE_TIME_TICKS, 0, {"0"},
//	currentUptime, NULL},

	// sysContact Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 4, 0}, 
	SNMPDTYPE_OCTET_STRING, 30, {"support@wiznet.co.kr"}, 
	NULL, NULL},

	// sysName Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 5, 0}, 
	SNMPDTYPE_OCTET_STRING, 30, {"http://www.iwiznet.cn"}, 
	NULL, NULL},

	// Location Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 6, 0}, 
	SNMPDTYPE_OCTET_STRING, 30, {"WIZnet ShenZhen Office"},
	NULL, NULL},

	// SysServices
	{8, {0x2b, 6, 1, 2, 1, 1, 7, 0}, 
	SNMPDTYPE_INTEGER, 4, {""}, 
	NULL, NULL},

	// WIZnet LED 
	{8, {0x2b, 6, 1, 4, 1, 0, 1, 0}, 
	SNMPDTYPE_OCTET_STRING, 30, {""},
	getWIZnetLed, NULL},

	{8, {0x2b, 6, 1, 4, 1, 0, 2, 0}, 
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, setWIZnetLed}
};

const int32 maxData = (sizeof(snmpData) / sizeof(dataEntryType));

int32 wiznetLedStatus = 0;
void initTable()
{ 
//	startTime = time(NULL);

	wiznetLedStatus=1;//LED_ON
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);			
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	
	snmpData[6].u.intval = 5;
	snmpData[7].u.intval = 0;
	snmpData[8].u.intval = 0;
}

void currentUptime(void *ptr, uint8 *len)
{
   time_t curTime;
   curTime = time(NULL);
	*(uint32 *)ptr = (uint32)(curTime - startTime) * 100;
	*len = 4;
	*(uint32 *)ptr = (uint32)(0);// just for test
	*len = 4;
	
}
/**
  * @brief  得到led灯状态
  * @param  ptr			- 存放状态指针变量
						len     - 字符串长度
  * @retval 无
  */
void getWIZnetLed(void *ptr, uint8 *len)
{
	if ( wiznetLedStatus==0 )	
      *len = sprintf((int8 *)ptr, "LED Off");
	else	
      *len = sprintf((int8 *)ptr, "LED On");
}

/**
  * @brief  设置led灯状态
  * @param  val			- 状态值

  * @retval 无
  */
void setWIZnetLed(int32 val)
{
	wiznetLedStatus = val;
	if ( wiznetLedStatus==0 )//LED OFF
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_0);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
	}
	else//LED ON
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);			
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);	
	}		
}

/**
  * @brief  用户snmp执行过程
  * @param  无

  * @retval 无
  */
void UserSnmpDemo(void)
{
	WDEBUG("\r\n\r\nStart UserSnmpDemo");
	SnmpXInit();	
	{
		dataEntryType enterprise_oid = {8, {0x2b, 6, 1, 4, 1, 0, 0x10, 0}, SNMPDTYPE_OBJ_ID, 8, {"\x2b\x06\x01\x04\x01\x00\x10\x00"},	NULL, NULL};
		dataEntryType trap_oid1 = {8, {0x2b, 6, 1, 4, 1, 0, 11, 0}, SNMPDTYPE_OCTET_STRING, 30, {""}, NULL, NULL};
		dataEntryType trap_oid2 = {8, {0x2b, 6, 1, 4, 1, 0, 12, 0}, SNMPDTYPE_INTEGER, 4, {""}, NULL, NULL};
		strcpy((int8*)trap_oid1.u.octetstring, "Alert!!!");
		trap_oid2.u.intval = 123456;
		SnmpXTrapSend("192.168.1.102", "192.168.1.88", "public", enterprise_oid, 1, 0, 0);
		SnmpXTrapSend("192.168.1.102", "192.168.1.88", "public", enterprise_oid, 6, 0, 2, &trap_oid1, &trap_oid2);
/*(int8* managerIP, int8* agentIP, int8* community, enterprise_oid, genericTrap,specificTrap,va_count, ...)*/
	}
	SnmpXDaemon();
}
/*
#ifdef WIN32
int32 main(int32 argc, int8 *argv[])
{
	UserSnmpDemo();
	return 0;
}
#endif
*/


