#include "socket.h"
#include "dhcp.h"
#include "ip_config.h"
/* If you want to display debug & procssing message, Define _DHCP_DEBUG_ in dhcp.h */

#ifdef _DHCP_DEBUG_
   #include <stdio.h>
#endif   

uint32_t toggle = 1;
uint8_t DHCP_SOCKET;                      // DHCP的Socket数 
uint8_t DHCP_SIP[4];                      // DHCP服务器IP地址

// Network information from DHCP Server
uint8_t OLD_allocated_ip[4]   = {0, };    // 之前得到的IP地址
uint8_t DHCP_allocated_ip[4]  = {0, };    // 从DHCP获取的IP地址
uint8_t DHCP_allocated_gw[4]  = {0, };    // 从DHCP获取的网关地址
uint8_t DHCP_allocated_sn[4]  = {0, };    // 从DHCP获取的子网掩码地址
uint8_t DHCP_allocated_dns[4] = {0, };    // 从DHCP获取的dns地址

int8_t   dhcp_state        = STATE_DHCP_INIT;   // DHCP状态
int8_t   dhcp_retry_count  = 0;                 

uint32_t dhcp_lease_time   			= INFINITE_LEASETIME;
volatile uint32_t dhcp_tick_1s      = 0;                 // unit 1 second
uint32_t dhcp_tick_next    			= DHCP_WAIT_TIME ;
uint32_t DHCP_XID;      // Any number
RIP_MSG* pDHCPMSG;      // Buffer pointer for DHCP processing
uint8_t HOST_NAME[] = DCHP_HOST_NAME;  
uint8_t DHCP_CHADDR[6]; // DHCP Client MAC address.

/* Callback handler */
void (*dhcp_ip_assign)(void)   = default_ip_assign;     /* handler to be called when the IP address from DHCP server is first assigned */
void (*dhcp_ip_update)(void)   = default_ip_update;     /* handler to be called when the IP address from DHCP server is updated */
void (*dhcp_ip_conflict)(void) = default_ip_conflict;   /* handler to be called when the IP address from DHCP server is conflict */

void reg_dhcp_cbfunc(void(*ip_assign)(void), void(*ip_update)(void), void(*ip_conflict)(void));

/**
*@brief		设置从DHCP服务器获取的IP信息 
*@param		无
*@return	无
*/
void default_ip_assign(void)
{
   setSIPR(DHCP_allocated_ip);
   setSUBR(DHCP_allocated_sn);
   setGAR (DHCP_allocated_gw);
}

/**
*@brief		更新系统默认的IP信息
*@param		无
*@return	无
*/
void default_ip_update(void)
{

   setMR(MR_RST);	/* W7500软复位 */
   getMR(); // for delay
   default_ip_assign();
   setSHAR(DHCP_CHADDR);
}

/**
*@brief		IP地址冲突
*@param		无
*@return	无
*/
void default_ip_conflict(void)
{

	setMR(MR_RST);	/* W7500软复位 */
	getMR(); // for delay
	setSHAR(DHCP_CHADDR);
}

/* register the call back func. */
void reg_dhcp_cbfunc(void(*ip_assign)(void), void(*ip_update)(void), void(*ip_conflict)(void))
{
   dhcp_ip_assign   = default_ip_assign;
   dhcp_ip_update   = default_ip_update;
   dhcp_ip_conflict = default_ip_conflict;
   if(ip_assign)   dhcp_ip_assign = ip_assign;
   if(ip_update)   dhcp_ip_update = ip_update;
   if(ip_conflict) dhcp_ip_conflict = ip_conflict;
}

/* make the common DHCP message */
/**
*@brief		整理基本的DHCP包信息
*@param		无
*@return	无
*/
void makeDHCPMSG(void)
{
   uint8_t  bk_mac[6];
   uint8_t* ptmp;
   uint8_t  i;
   getSHAR(bk_mac);
	pDHCPMSG->op      = DHCP_BOOTREQUEST;				//若是client发送给server,设为1,反向为2
	pDHCPMSG->htype   = DHCP_HTYPE10MB;					//硬件类型
	pDHCPMSG->hlen    = DHCP_HLENETHERNET;			//硬件长度
	pDHCPMSG->hops    = DHCP_HOPS;							//	若数据包需要通过router发送，没站加1，若在同一个网段内，为0
	ptmp              = (uint8_t*)(&pDHCPMSG->xid);//事物ID
	*(ptmp+0)         = (uint8_t)((DHCP_XID & 0xFF000000) >> 24);
	*(ptmp+1)         = (uint8_t)((DHCP_XID & 0x00FF0000) >> 16);
  *(ptmp+2)         = (uint8_t)((DHCP_XID & 0x0000FF00) >>  8);
	*(ptmp+3)         = (uint8_t)((DHCP_XID & 0x000000FF) >>  0);   
	pDHCPMSG->secs    = DHCP_SECS;			//值开始获取地址和更新进行后的时间
	ptmp              = (uint8_t*)(&pDHCPMSG->flags);	
	*(ptmp+0)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0xFF00) >> 8);
	*(ptmp+1)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0x00FF) >> 0);

	for(i = 0; i < 4; i++)
	{
		pDHCPMSG->ciaddr[i] = 0;			//用户IP地址
		pDHCPMSG->yiaddr[i] = 0;			//客户IP地址
		pDHCPMSG->siaddr[i] = 0;				//用于bootstrap过程的IP地址
		pDHCPMSG->giaddr[i] = 0;				//转发代理IP地址		
	}
	for(i = 0; i < 6; i++)					//client的硬件地址
	{
		pDHCPMSG->chaddr[i] = DHCP_CHADDR[i];	
	}
	for (i = 6; i < 16; i++)  pDHCPMSG->chaddr[i] = 0;
	for (i = 0; i < 64; i++)  pDHCPMSG->sname[i]  = 0;//可选server的名称，以0x00结束
	for (i = 0; i < 128; i++) pDHCPMSG->file[i]   = 0;//启动文件名
	// MAGIC_COOKIE
	pDHCPMSG->OPT[0] = (uint8_t)((MAGIC_COOKIE & 0xFF000000) >> 24);
	pDHCPMSG->OPT[1] = (uint8_t)((MAGIC_COOKIE & 0x00FF0000) >> 16);
	pDHCPMSG->OPT[2] = (uint8_t)((MAGIC_COOKIE & 0x0000FF00) >>  8);
	pDHCPMSG->OPT[3] = (uint8_t) (MAGIC_COOKIE & 0x000000FF) >>  0;
}

/**
*@brief		向DHCP服务器发送DISCOVER包
*@param		无
*@return	无
*/
void send_DHCP_DISCOVER(void)
{
	uint16_t i;
	uint8_t ip[4];
	uint16_t k = 0;
  uint32_t ret;
   
   makeDHCPMSG();
   k = 4;     // beacaue MAGIC_COOKIE already made by makeDHCPMSG()  
	// Option Request Param
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DISCOVER;	
	// Client identifier
	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];	
	// host name
	pDHCPMSG->OPT[k++] = hostName;
	pDHCPMSG->OPT[k++] = 0;          // fill zero length of hostname 
	for(i = 0 ; HOST_NAME[i] != 0; i++)
   	pDHCPMSG->OPT[k++] = HOST_NAME[i];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
	pDHCPMSG->OPT[k - (i+3+1)] = i+3; // length of hostname
	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x06;	// length of request
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = endOption;
	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;	
	for (i = 0; i < 4; i++) ip[i] = 255;
	#ifdef _DHCP_DEBUG_
	printf("> Send DHCP_DISCOVER\r\n");
	#endif
	ret= sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
	#ifdef _DHCP_DEBUG_
	printf("> %d, %d\r\n", ret, RIP_MSG_SIZE);
	#endif
}

/**
*@brief		向DHCP服务器发送请求
*@param		无
*@return	无
*/
void send_DHCP_REQUEST(void)
{
	int i;
	uint8_t ip[4];
	uint16_t k = 0;
   makeDHCPMSG();
   if(dhcp_state == STATE_DHCP_LEASED || dhcp_state == STATE_DHCP_REREQUEST)
   {
   	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
   	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);
		
		 for (i = 0; i < 4; i++)
		 {
				pDHCPMSG->ciaddr[i] = DHCP_allocated_ip[i];	
				ip[i] = DHCP_SIP[i];			 
		 }  	   	   	
   }
   else
   {
		 for (i = 0; i < 4; i++) ip[i] = 255;
   }   
   k = 4;      // beacaue MAGIC_COOKIE already made by makeDHCPMSG()	
	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_REQUEST;
	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
   if(ip[3] == 255)  // if(dchp_state == STATE_DHCP_LEASED || dchp_state == DHCP_REREQUEST_STATE)
   {
		pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[0];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[1];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[2];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[3];
		pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = DHCP_SIP[0];
		pDHCPMSG->OPT[k++] = DHCP_SIP[1];
		pDHCPMSG->OPT[k++] = DHCP_SIP[2];
		pDHCPMSG->OPT[k++] = DHCP_SIP[3];
	}
	// host name
	pDHCPMSG->OPT[k++] = hostName;
	pDHCPMSG->OPT[k++] = 0; // length of hostname
	for(i = 0 ; HOST_NAME[i] != 0; i++)
  pDHCPMSG->OPT[k++] = HOST_NAME[i];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
	pDHCPMSG->OPT[k - (i+3+1)] = i+3; // length of hostname
	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x08;
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = performRouterDiscovery;
	pDHCPMSG->OPT[k++] = staticRoute;
	pDHCPMSG->OPT[k++] = endOption;
	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;
	#ifdef _DHCP_DEBUG_
	printf("> Send DHCP_REQUEST\r\n");
	#endif
	sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
}

/**
*@brief		SEND DHCP DHCPDECLINE
*@param		无
*@return	无
*/
void send_DHCP_DECLINE(void)
{
	int i;
	uint8_t ip[4];
	uint16_t k = 0;
	
	makeDHCPMSG();
   k = 4;      // beacaue MAGIC_COOKIE already made by makeDHCPMSG()
	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);
	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DECLINE;
	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
	pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[0];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[1];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[2];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[3];
	pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = DHCP_SIP[0];
	pDHCPMSG->OPT[k++] = DHCP_SIP[1];
	pDHCPMSG->OPT[k++] = DHCP_SIP[2];
	pDHCPMSG->OPT[k++] = DHCP_SIP[3];
	pDHCPMSG->OPT[k++] = endOption;
	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;	
	for (i = 0; i < 4; i++) ip[i] =0xFF ;
	#ifdef _DHCP_DEBUG_
	printf("\r\n> Send DHCP_DECLINE\r\n");
	#endif
	sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
}

/**
*@brief		解析DHCP服务器应答信息
*@param		无
*@return	无
*/
int8_t parseDHCPMSG(void)
{
	uint8_t svr_addr[6];
	uint16_t  svr_port;
	uint16_t len;

	uint8_t * p;
	uint8_t * e;
	uint8_t type;
	uint8_t opt_len;
   
   if((len = getSn_RX_RSR(DHCP_SOCKET)) > 0)
   {
			len = recvfrom(DHCP_SOCKET, (uint8_t *)pDHCPMSG, len, svr_addr, &svr_port);
			#ifdef _DHCP_DEBUG_   
      printf("DHCP message : %d.%d.%d.%d(%d) %d received. \r\n",svr_addr[0],svr_addr[1],svr_addr[2], svr_addr[3],svr_port, len);
			#endif   
   }
   else return 0;
	if (svr_port == DHCP_SERVER_PORT) 
	{
		if ( (pDHCPMSG->chaddr[0] != DHCP_CHADDR[0]) || (pDHCPMSG->chaddr[1] != DHCP_CHADDR[1]) ||
		     (pDHCPMSG->chaddr[2] != DHCP_CHADDR[2]) || (pDHCPMSG->chaddr[3] != DHCP_CHADDR[3]) ||
		     (pDHCPMSG->chaddr[4] != DHCP_CHADDR[4]) || (pDHCPMSG->chaddr[5] != DHCP_CHADDR[5])   )
         return 0;
      type = 0;
		p = (uint8_t *)(&pDHCPMSG->op);
		p = p + 240;      // 240 = sizeof(RIP_MSG) + MAGIC_COOKIE size in RIP_MSG.opt - sizeof(RIP_MSG.opt)
		e = p + (len - 240);

		while ( p < e ) 
		{
			switch ( *p ) 
			{
   			case endOption :
						p = e;   // for break while(p < e)
   			break;
        case padOption :
						p++;
   			break;
   			case dhcpMessageType :
						p++;
						p++;
						type = *p++;
   			break;
   			case subnetMask :
						p++;
						p++;
						DHCP_allocated_sn[0] = *p++;
						DHCP_allocated_sn[1] = *p++;
						DHCP_allocated_sn[2] = *p++;
						DHCP_allocated_sn[3] = *p++;
   			break;
   			case routersOnSubnet :
						p++;
						opt_len = *p++;       
						DHCP_allocated_gw[0] = *p++;
						DHCP_allocated_gw[1] = *p++;
						DHCP_allocated_gw[2] = *p++;
						DHCP_allocated_gw[3] = *p++;
						p = p + (opt_len - 4);
   			break;
   			case dns :
						p++;                  
						opt_len = *p++;       
						DHCP_allocated_dns[0] = *p++;
						DHCP_allocated_dns[1] = *p++;
						DHCP_allocated_dns[2] = *p++;
						DHCP_allocated_dns[3] = *p++;
						p = p + (opt_len - 4);
   			break;
   			case dhcpIPaddrLeaseTime :
						p++;
						opt_len = *p++;
						dhcp_lease_time  = *p++;
						dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
						dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
						dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
						#ifdef _DHCP_DEBUG_  
								 dhcp_lease_time = 10;
						#endif
   			break;
   			case dhcpServerIdentifier :
						p++;
						opt_len = *p++;
						DHCP_SIP[0] = *p++;
						DHCP_SIP[1] = *p++;
						DHCP_SIP[2] = *p++;
						DHCP_SIP[3] = *p++;
   			break;
   			default :
   				p++;
   				opt_len = *p++;
   				p += opt_len;
   				break;
			} // switch
		} // while
	} // if
	return	type;
}

/**
*@brief		执行DHCP过程
*@param		无
*@return	无
*/
uint8_t DHCP_run(void)
{
	uint8_t  type;
	uint8_t  ret;
	uint8_t  i;
	if(dhcp_state == STATE_DHCP_STOP) return DHCP_STOPPED;

	if(getSn_SR(DHCP_SOCKET) != SOCK_UDP)											//开启UDP模式
	   socket(DHCP_SOCKET, Sn_MR_UDP, DHCP_CLIENT_PORT, 0x00);

	ret = DHCP_RUNNING;
	type = parseDHCPMSG();

	switch ( dhcp_state ) 
	{
	   case STATE_DHCP_INIT     :														//DHCP初始化状态
			 for (i = 0; i < 4; i++) DHCP_allocated_ip[i] = 0;
   		send_DHCP_DISCOVER();																//发送DISCOVER包
   		dhcp_state = STATE_DHCP_DISCOVER;
   		break;
		case STATE_DHCP_DISCOVER :														//DISCOVER状态
			if (type == DHCP_OFFER)															
			{
				#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_OFFER\r\n");
				#endif
				for (i = 0; i < 4; i++) 
				{
            DHCP_allocated_ip[i] = pDHCPMSG->yiaddr[i];
				}
				send_DHCP_REQUEST();
				dhcp_state = STATE_DHCP_REQUEST;
			} else ret = check_DHCP_timeout();
         break;

		case STATE_DHCP_REQUEST :														//REQUET请求状态
			if (type == DHCP_ACK) 														//DHCP请求响应成功
			{
				#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_ACK\r\n");
				#endif
				printf("> Receive !!!\r\n");

				if (check_DHCP_leasedIP()) 											//IP地址租赁成功
				{
					// Network info assignment from DHCP
					dhcp_ip_assign();															//从DHCP服务器分配IP地址
					reset_DHCP_timeout();													//	复位超时时间
					dhcp_state = STATE_DHCP_LEASED;									//
				} 
				else 																						
				{
					// IP address conflict occurred
					reset_DHCP_timeout();													//复位超时时间
					dhcp_ip_conflict();														//判断iP地址是否冲突
				    dhcp_state = STATE_DHCP_INIT;
				}
			}
			else if (type == DHCP_NAK) 												//DHCP请求响应不成功
			{
				#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_NACK\r\n");
				#endif
				reset_DHCP_timeout();														//复位超时时间
				dhcp_state = STATE_DHCP_DISCOVER;
			} 
			else ret = check_DHCP_timeout();									//判断请求是否超时
		break;
		case STATE_DHCP_LEASED :														//IP地址租赁状态
		   ret = DHCP_IP_LEASED;
			if ((dhcp_lease_time != INFINITE_LEASETIME) && ((dhcp_lease_time/2) < dhcp_tick_1s)) 
			{			
				#ifdef _DHCP_DEBUG_
 				printf("> Maintains the IP address \r\n");
				#endif
				type = 0;
				for (i = 0; i < 4; i++) 
				{
				OLD_allocated_ip[i] = DHCP_allocated_ip[i];			//保存得到的IP地址
				}
				DHCP_XID++;

				send_DHCP_REQUEST();														//发送请求
				reset_DHCP_timeout();
				dhcp_state = STATE_DHCP_REREQUEST;
			}
		break;
		case STATE_DHCP_REREQUEST :													//重新发送请求，判断IP地址是否成功分配
		   ret = DHCP_IP_LEASED;
			if (type == DHCP_ACK) 														//重传请求应答成功
			{
				dhcp_retry_count = 0;
				if (OLD_allocated_ip[0] != DHCP_allocated_ip[0] || 
				    OLD_allocated_ip[1] != DHCP_allocated_ip[1] ||
				    OLD_allocated_ip[2] != DHCP_allocated_ip[2] ||
				    OLD_allocated_ip[3] != DHCP_allocated_ip[3]) 
				{
					ret = DHCP_IP_CHANGED;
					dhcp_ip_update();
               #ifdef _DHCP_DEBUG_
                  printf(">IP changed.\r\n");
               #endif
					
				}
         #ifdef _DHCP_DEBUG_
            else printf(">IP is continued.\r\n");
         #endif            				
				reset_DHCP_timeout();
				dhcp_state = STATE_DHCP_LEASED;
			} 
			else if (type == DHCP_NAK) 													//重传请求应答不成功
			{
				#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_NACK, Failed to maintain ip\r\n");
				#endif
				reset_DHCP_timeout();															//复位超时时间
				dhcp_state = STATE_DHCP_DISCOVER;
			} else ret = check_DHCP_timeout();
	   	break;
		default :
   		break;
	}
	return ret;
}

/**
*@brief		DHCP获取停止
*@param		无
*@return	无
*/
void  DHCP_stop(void)
{
   close(DHCP_SOCKET);
   dhcp_state = STATE_DHCP_STOP;
}

/**
*@brief		检查DHCP超时状态
*@param		无
*@return	无
*/
uint8_t check_DHCP_timeout(void)
{
	uint8_t ret = DHCP_RUNNING;
	
	if (dhcp_retry_count < MAX_DHCP_RETRY) 
	{
		if (dhcp_tick_next < dhcp_tick_1s) 
		{
			switch ( dhcp_state ) 						//判断DHCP状态
			{
				case STATE_DHCP_DISCOVER :			//发送DISCOVER包状态
						send_DHCP_DISCOVER();				//向DHCP服务器发送DISCOVER包
				break;		
				case STATE_DHCP_REQUEST :				//发送REQUEST请求状态
						send_DHCP_REQUEST();				//发送REQUEST请求包
				break;
				case STATE_DHCP_REREQUEST :			//再次发送REQUEST请求状态	
						send_DHCP_REQUEST();				//发送REQUEST请求包
				break;
				default :
				break;
			}
			dhcp_tick_1s = 0;
			dhcp_tick_next = dhcp_tick_1s + DHCP_WAIT_TIME;
			dhcp_retry_count++;
		}
	} 
	else 																		// DHCP获取超时
	{ 
		switch(dhcp_state) 
		{
			case STATE_DHCP_DISCOVER:						//发送Discover包状态
				dhcp_state = STATE_DHCP_INIT;
				ret = DHCP_FAILED;
			break;
			case STATE_DHCP_REQUEST:						//发送REQUEST请求状态
			case STATE_DHCP_REREQUEST:					//重新发送REQUEST请求状态
				send_DHCP_DISCOVER();							//	发送DISCOVER包
				dhcp_state = STATE_DHCP_DISCOVER;
			break;
			default :
				break;
		}
		reset_DHCP_timeout();									//复位超时时间
	}
	return ret;
}

/**
*@brief		检查DHCP分配IP地址是否被租赁
*@param		无
*@return	无
*/
int8_t check_DHCP_leasedIP(void)
{
	uint8_t tmp;
	int32_t ret;
	tmp = getRCR();													//获取重试计数寄存器值
	setRCR(0x03);														//设置重试计数寄存器

	ret = sendto(DHCP_SOCKET, (uint8_t *)"CHECK_IP_CONFLICT", 17, DHCP_allocated_ip, 5000);
	setRCR(tmp);
	if(ret == SOCKERR_TIMEOUT) 
	{
		#ifdef _DHCP_DEBUG_
		printf("\r\n> Check leased IP - OK\r\n");
		#endif
		return 1;
	} 
	else 
	{
		send_DHCP_DECLINE();
		ret = dhcp_tick_1s;
		while((dhcp_tick_1s - ret) < 2);   // wait for 1s over; wait to complete to send DECLINE message;
		return 0;
	}
}	

/**
*@brief		DHCP初始化
*@param		无
*@return	无
*/
void DHCP_init(uint8_t s, uint8_t * buf)
{
   uint8_t zeroip[4] = {0,0,0,0};
   getSHAR(DHCP_CHADDR);
   if((DHCP_CHADDR[0] | DHCP_CHADDR[1]  | DHCP_CHADDR[2] | DHCP_CHADDR[3] | DHCP_CHADDR[4] | DHCP_CHADDR[5]) == 0x00)
   {
      printf("DHCP_init-set MAC\r\n");
		  // assing temporary mac address, you should be set SHAR before call this function. 
      DHCP_CHADDR[0] = 0x00;
      DHCP_CHADDR[1] = 0x08;
      DHCP_CHADDR[2] = 0xdc;      
      DHCP_CHADDR[3] = 0x00;
      DHCP_CHADDR[4] = 0x00;
      DHCP_CHADDR[5] = 0x00; 
      setSHAR(DHCP_CHADDR);     
   }
	DHCP_SOCKET = s; // SOCK_DHCP
	pDHCPMSG = (RIP_MSG*)buf;
	DHCP_XID = 0x12345678;

	setSIPR(zeroip);
	setSIPR(zeroip);
	setGAR(zeroip);

	reset_DHCP_timeout();
	dhcp_state = STATE_DHCP_INIT;
}

/**
*@brief		复位DHCP超时状态，重新计数
*@param		无
*@return	无
*/
void reset_DHCP_timeout(void)
{
	dhcp_tick_1s = 0;
	dhcp_tick_next = DHCP_WAIT_TIME;
	dhcp_retry_count = 0;
}

/**
*@brief		DHCP计数处理
*@param		无
*@return	无
*/
void DHCP_time_handler(void)
{
	dhcp_tick_1s++;
}

/**
*@brief		得到从DHCP服务器获取的IP地址
*@param		无
*@return	无
*/
void getIPfromDHCP(uint8_t* ip)
{
	uint8_t i;
	for (i = 0; i < 4; i++) 
	ip[i] = DHCP_allocated_ip[i];
}

/**
*@brief		得到从DHCP服务器获取的网关
*@param		无
*@return	无
*/
void getGWfromDHCP(uint8_t* ip)
{	
	uint8_t i;
	for (i = 0; i < 4; i++) 
	ip[i] =DHCP_allocated_gw[i];		
}

/**
*@brief		得到从DHCP服务器获取的子网掩码
*@param		无
*@return	无
*/
void getSNfromDHCP(uint8_t* ip)
{
		uint8_t i;
		for (i = 0; i < 4; i++) 
   ip[i] = DHCP_allocated_sn[i];      
}

/**
*@brief		得到从DHCP服务器获取的DNS
*@param		无
*@return	无
*/
void getDNSfromDHCP(uint8_t* ip)
{
		uint8_t i;
		for (i = 0; i < 4; i++) 
   ip[i] = DHCP_allocated_dns[i];        
}

/**
*@brief		得到从DHCP服务器获取IP租赁时间
*@param		无
*@return	无
*/
uint32_t getDHCPLeasetime(void)
{
	return dhcp_lease_time;
}

/**
*@brief		执行DHCP获取过程
*@param		无
*@return	无
*/
void do_dhcp(void)
{
	uint8_t tmp[8];
	uint32_t my_dhcp_retry = 0;
	switch(DHCP_run())											//判断DHCP运行状态
	{
		case DHCP_IP_ASSIGN:									//首次分配IP地址状态
		case DHCP_IP_CHANGED:									//从DHCP服务器获取新的IP地址状态
				toggle = 1;
				if(toggle)
				{
					toggle = 0;
					close(SOCK_TCPS); 
				}  						
		break;
		case DHCP_IP_LEASED:								//成功租赁到IP地址状态
				if(toggle)
				{
					set_w7500_ip();								//设置IP地址
					toggle = 0;
				}  
		break;
		case DHCP_FAILED:										//	DHCP 获取失败
				my_dhcp_retry++;
				if(my_dhcp_retry > MY_MAX_DHCP_RETRY)
				{
					#if DEBUG_MODE != DEBUG_NO
					printf(">> DHCP %d Failed\r\n",my_dhcp_retry);
					#endif
					my_dhcp_retry = 0;
					DHCP_stop();      // if restart, recall DHCP_init()
				}
		break;
		default:
		break;
	}	
}


