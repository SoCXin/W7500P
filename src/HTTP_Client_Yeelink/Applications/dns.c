/**
******************************************************************************
* @file    			DNS.c
* @author  			WIZnet Software Team 
* @version 			V1.0
* @date    			2015-xx-xx
* @brief   			���������ͻ��˺��� ͨ����������  Domain_name�ɻ����IP��ַ   
******************************************************************************
*/
#include <string.h>
#include <stdlib.h>
#include "ip_config.h"
#include "time_delay.h"
#include "socket.h"
#include "dns.h"

#ifdef _DNS_DEBUG_
   #include <stdio.h>
#endif

uint8_t* pDNSMSG;       // DNS message buffer
uint8_t  DNS_SOCKET;    // SOCKET number for DNS
uint16_t DNS_MSGID;     // DNS message ID

uint32_t dns_1s_tick;   // for timout of DNS processing

/* converts uint16_t from network buffer to a host byte order integer. */
uint16_t get16(uint8_t * s)
{
	uint16_t i;
	i = *s++ << 8;
	i = i + *s;
	return i;
}

/* copies uint16_t to the network buffer with network byte order. */
uint8_t * put16(uint8_t * s, uint16_t i)
{
	*s++ = i >> 8;
	*s++ = i;
	return s;
}

 /*              ��������
 * Arguments   : msg        - Ӧ����Ϣָ�����
 *               compressed - Ӧ����Ϣ��������ָ�����.
 *               buf        - is a pointer to the buffer for the human-readable form name.
 *               len        - buffer���ֵ.
 * Returns     : 						ѹ�����ĵĳ���
 */
int parse_name(uint8_t * msg, uint8_t * compressed, char * buf, int16_t len)
{
	uint16_t slen;					/* ��ǰ���䳤�� */
	uint8_t * cp;
	int clen = 0;						/* ѹ�����ܳ��� */
	int indirect = 0;				/* ����������Ҫ���� */
	int nseg = 0;						/* ���ƶ����ܸ��� */

	cp = compressed;

	for (;;)
	{
		slen = *cp++;					/* �����䳤�� */
		if (!indirect) clen++;
		if ((slen & 0xc0) == 0xc0)
		{
			if (!indirect)
			clen++;
			indirect = 1;
			/* Follow indirection */
			cp = &msg[((slen & 0x3f)<<8) + *cp];
			slen = *cp++;
		}
		if (slen == 0)			break;	/* zero length == all done */
		len -= slen + 1;
		if (len < 0) return -1;
		if (!indirect) clen += slen;
		while (slen-- != 0) *buf++ = (char)*cp++;
		*buf++ = '.';
		nseg++;
	}
	if (nseg == 0)
	{														/* ������;��Ϊ�����ĵ���� */
		*buf++ = '.';
		len--;
	}
	*buf++ = '\0';
	len--;
	return clen;								/* ����ѹ����Ϣ���� */
}

/**
*@brief		 ������ܺ�����������Ϣ�����¼
*@param		 msg - ָ��ظ���Ϣ��ָ��
           cp  - ָ�������¼��ָ��
*@return	 ������һ����¼ָ��
*/
uint8_t * dns_question(uint8_t * msg, uint8_t * cp)
{
	int len;
	char name[MAXCNAME];
	len = parse_name(msg, cp, name, MAXCNAME);
	if (len == -1) return 0;
	cp += len;
	cp += 2;		/* type */
	cp += 2;		/* class */
	return cp;
}

/**
*@brief		 ������ܺ��������ظ���Ϣ�Ļش��¼
*@param		 msg - ָ��ظ���Ϣ��ָ��
           cp  - ���ջش��¼��ָ��
*@return	 ������һ���ظ���¼ָ��
*/
uint8_t * dns_answer(uint8_t * msg, uint8_t * cp, uint8_t * ip_from_dns)
{
	int len, type;
	char name[MAXCNAME];
	len = parse_name(msg, cp, name, MAXCNAME);
	if (len == -1) return 0;
	cp += len;
	type = get16(cp);
	cp += 2;		/* type */
	cp += 2;		/* class */
	cp += 4;		/* ttl */
	cp += 2;		/* len */
	switch (type)
	{
		case TYPE_A:		/* Just read the address directly into the structure */
				ip_from_dns[0] = *cp++;
				ip_from_dns[1] = *cp++;
				ip_from_dns[2] = *cp++;
				ip_from_dns[3] = *cp++;
		break;
		case TYPE_CNAME:
		case TYPE_MB:
		case TYPE_MG:
		case TYPE_MR:
		case TYPE_NS:
		case TYPE_PTR:		/* These types all consist of a single domain name */
				len = parse_name(msg, cp, name, MAXCNAME);		/* convert it to ascii format */
				if (len == -1) return 0;
				cp += len;
		break;
		case TYPE_HINFO:
				len = *cp++;
				cp += len;
				len = *cp++;
				cp += len;
		break;
		case TYPE_MX:
				cp += 2;
				len = parse_name(msg, cp, name, MAXCNAME);		/* Get domain name of exchanger */
				if (len == -1) return 0;
				cp += len;
		break;
		case TYPE_SOA:		/* Get domain name of name server */
				len = parse_name(msg, cp, name, MAXCNAME);
				if (len == -1) return 0;
				cp += len;
				len = parse_name(msg, cp, name, MAXCNAME);		/* Get domain name of responsible person */
				if (len == -1) return 0;
				cp += len;
				cp += 4;
				cp += 4;
				cp += 4;
				cp += 4;
				cp += 4;
		break;
		case TYPE_TXT:		/* Just stash */
		break;
		default:		/* Ignore */
		break;
	}
	return cp;
}

/**
*@brief		 ������ܺ�����������DNS�������Ļظ���Ϣ
*@param		 dhdr - ָ��DNS��Ϣͷ��ָ��
           buf  - ���ջظ���Ϣ
           len  - �ظ���Ϣ�ĳ���
* Returns    -1 - ��������̫��
*             0 - ʧ�� (��ʱ���ߴ���)
*             1 - �ɹ� 
*/
int8_t parseDNSMSG(struct dhdr * pdhdr, uint8_t * pbuf, uint8_t * ip_from_dns)
{
	uint16_t tmp;
	uint16_t i;
	uint8_t * msg;
	uint8_t * cp;

	msg = pbuf;
	memset(pdhdr, 0, sizeof(pdhdr));
	pdhdr->id = get16(&msg[0]);
	tmp = get16(&msg[2]);
	if (tmp & 0x8000) pdhdr->qr = 1;
	pdhdr->opcode = (tmp >> 11) & 0xf;
	if (tmp & 0x0400) pdhdr->aa = 1;
	if (tmp & 0x0200) pdhdr->tc = 1;
	if (tmp & 0x0100) pdhdr->rd = 1;
	if (tmp & 0x0080) pdhdr->ra = 1;
	pdhdr->rcode = tmp & 0xf;
	pdhdr->qdcount = get16(&msg[4]);
	pdhdr->ancount = get16(&msg[6]);
	pdhdr->nscount = get16(&msg[8]);
	pdhdr->arcount = get16(&msg[10]);
	/* Now parse the variable length sections */
	cp = &msg[12];
	for (i = 0; i < pdhdr->qdcount; i++)	/* Question section */
	{
		cp = dns_question(msg, cp);
		#ifdef _DNS_DEUBG_
      printf("MAX_DOMAIN_NAME is too small, it should be redfine in dns.h"
		#endif
		if(!cp) return -1;
	}
	for (i = 0; i < pdhdr->ancount; i++)	/* Answer section */
	{
			cp = dns_answer(msg, cp, ip_from_dns);
			#ifdef _DNS_DEUBG_
      printf("MAX_DOMAIN_NAME is too small, it should be redfine in dns.h"
		#endif
		if(!cp) return -1;
	}
	for (i = 0; i < pdhdr->nscount; i++)	/* Name server (authority) section */
	{;}
	for (i = 0; i < pdhdr->arcount; i++)	/* Additional section */
	{;}
	if(pdhdr->rcode == 0) return 1;		// No error
	else return 0;
}

/**
*@brief		 	������ܺ�����ѯDNS������Ϣ����������DNS�������Ļظ�
*@param			s    - DNS������socket��
						name - Ҫ��������Ϣ
 *          buf  - DNS��Ϣ����ָ�����.
 *          len  - ������󳤶�.
*@return		�ɹ�: ����1, ʧ�� :���� -1
*/
int16_t dns_makequery(uint16_t op, char * name, uint8_t * buf, uint16_t len)
{
	uint8_t *cp;
	char *cp1;
	char sname[MAXCNAME];
	char *dname;
	uint16_t p;
	uint16_t dlen;
	cp = buf;
	DNS_MSGID++;
	cp = put16(cp, DNS_MSGID);
	p = (op << 11) | 0x0100;			/* Recursion desired */
	cp = put16(cp, p);
	cp = put16(cp, 1);
	cp = put16(cp, 0);
	cp = put16(cp, 0);
	cp = put16(cp, 0);
	strcpy(sname, name);
	dname = sname;
	dlen = strlen(dname);
	for (;;)
	{
		/* Look for next dot */
		cp1 = strchr(dname, '.');
		if (cp1 != NULL) len = cp1 - dname;	/* More to come */
		else len = dlen;			/* Last component */
		*cp++ = len;				/* Write length of component */
		if (len == 0) break;
		/* Copy component up to (but not including) dot */
		strncpy((char *)cp, dname, len);
		cp += len;
		if (cp1 == NULL)
		{
			*cp++ = 0;			/* Last one; write null and finish */
			break;
		}
		dname += len+1;
		dlen -= len+1;
	}
	cp = put16(cp, 0x0001);				/* type */
	cp = put16(cp, 0x0001);				/* class */
	return ((int16_t)((uint32_t)(cp) - (uint32_t)(buf)));
}

/*
 * Description : ������ܺ�����鳬ʱ
 **@param	     : ��.
 * Returns     : -1 - ������ʱ 
									0 - ��ʱ��������û�з�����ʱ
									1 - ��ʱû������û�з�����ʱ
 * Note        : timeout : retry count and timer both over.
 */
int8_t check_DNS_timeout(void)
{
	static uint8_t retry_count;
	if(dns_1s_tick >= DNS_WAIT_TIME)
	{
		dns_1s_tick = 0;
		if(retry_count >= MAX_DNS_RETRY) 
		{
			retry_count = 0;
			return -1; 															// ������ʱ
		}
		retry_count++;
		return 0; 																// ��ʱ������û�г�ʱ
	}
	return 1; 																	// ��ʱδ������û�г�ʱ
}

/* 
		DNS�ͻ��˳�ʼ��
*/
void DNS_init( uint8_t * buf)
{
	pDNSMSG = buf; // User's shared buffer
	DNS_MSGID = DNS_MSG_ID;
}

/* 
	DNS�ͻ�������
*/
int8_t DNS_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns)
{
	int8_t ret;
	struct dhdr dhp;
	uint8_t ip[4];
	uint16_t len, port;
	int8_t ret_check_timeout;  
  socket(DNS_SOCKET, Sn_MR_UDP,0,0);   		// ��DNS��Socket�˿�
	#ifdef _DNS_DEBUG_
	printf("> DNS Query to DNS Server: %d.%d.%d.%d\r\n",dns_ip[0],dns_ip[1],dns_ip[2],dns_ip[3]);
	#endif 
	len = dns_makequery(0, (char *)name, pDNSMSG, MAX_DNS_BUF_SIZE);
	sendto(DNS_SOCKET, pDNSMSG, len, dns_ip, IPPORT_DOMAIN);//��DNS���������ͽ�����Ϣ
	while (1)
	{
		if ((len = getSn_RX_RSR(DNS_SOCKET)) > 0)
		{
			if (len > MAX_DNS_BUF_SIZE) len = MAX_DNS_BUF_SIZE;
			len = recvfrom(DNS_SOCKET, pDNSMSG, len, ip, &port);//���շ�����Ӧ����Ϣ
      #ifdef _DNS_DEBUG_
	      printf("> Receive DNS message from %d.%d.%d.%d(%d). len = %d\r\n", ip[0], ip[1], ip[2], ip[3],port,len);
      #endif
         ret = parseDNSMSG(&dhp, pDNSMSG, ip_from_dns);
			break;
		}
		// Check Timeout
		ret_check_timeout = check_DNS_timeout();
		if (ret_check_timeout < 0) 
		{
			#ifdef _DNS_DEBUG_
				printf("> DNS Server is not responding : %d.%d.%d.%d\r\n", dns_ip[0], dns_ip[1], dns_ip[2], dns_ip[3]);
			#endif
			return 0; // timeout occurred
		}
		else if (ret_check_timeout == 0) 
		{
			#ifdef _DNS_DEBUG_
				printf("> DNS Timeout\r\n");
			#endif
			sendto(DNS_SOCKET, pDNSMSG, len, dns_ip, IPPORT_DOMAIN);
		}
	}
	close(DNS_SOCKET);//�ر�Socket
	return ret;
}

/* DNS TIMER HANDLER */
void DNS_time_handler(void)
{
	dns_1s_tick++;
}

int32_t do_dns(uint8_t sn, uint8_t* buf)
{
    int32_t ret;
		#ifdef _DNS_DEBUG_
    printf("\r\n===== DNS Servers =====\r\n");
//    printf("> DNS  : %d.%d.%d.%d\r\n", dns_server[0], dns_server[1], dns_server[2], dns_server[3]);
		#endif
    DNS_init(buf);
    if (((ret = DNS_run(dns_server, Domain_name, Domain_IP))>0))     // retry to 2nd DNS
    {
			// memcpy(ConfigMsg.rip,Domain_IP,4);//�ѽ�������IP��ַ���Ƹ�ConfigMsg.rip
     	printf("[%2s]'s IP address is: %d.%d.%d.%d\r\n",Domain_name,Domain_IP[0],Domain_IP[1],Domain_IP[2],Domain_IP[3]);
    }
    else
       printf("> DNS Failed\r\n");
		
		delay_ms(1000);
		#ifdef _DNS_DEBUG_
    if(ret>0)
    {
    //    printf("> Domain_IP : %d.%d.%d.%d\r\n", Domain_IP[0], Domain_IP[1], Domain_IP[2], Domain_IP[3]);
    }        
		#endif
    return ret;
}

