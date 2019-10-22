/**
******************************************************************************
* @file   		tcpc.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-xx-xx
* @brief   		TCP ��ʾ����
* @attention  
******************************************************************************
**/
#include <stdio.h>
#include "tcpc.h"
#include "socket.h"
#include "wizchip_conf.h"

/**
*@brief		TCP Client�ػ���ʾ������
*@param		��
*@return	��
*/
int32_t tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport,uint16_t local_port)
{
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0, sentsize=0;
   switch(getSn_SR(sn))														/*��ȡsocket��״̬*/
   {
			case SOCK_ESTABLISHED : 										/*socket�������ӽ���״̬*/
					if(getSn_IR(sn) & Sn_IR_CON)	
					{
						setSn_IR(sn, Sn_IR_CON);   						/*��������жϱ�־λ*/
					}
					if((size = getSn_RX_RSR(sn)) > 0) 
					{
						if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
						ret = recv(sn, buf, size);						/*��������Server������*/		
						if(ret <= 0) return ret;
						sentsize = 0;
						while(size != sentsize)
						{
							ret = send(sn, buf+sentsize, size-sentsize);/*��Server��������*/
							if(ret < 0) 
							{
								close(sn); 
								return ret;
							}
							sentsize += ret;
						}
					}
      break;
      case SOCK_CLOSE_WAIT : 											/*socket���ڵȴ��ر�״̬*/
         if((ret=disconnect(sn)) != SOCK_OK) return ret;
					#ifdef _LOOPBACK_DEBUG_
							printf("%d:Socket Closed\r\n", sn);
					#endif
      break;
      case SOCK_INIT :												/*socket���ڳ�ʼ��״̬*/
				#ifdef _LOOPBACK_DEBUG_
						printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
				#endif
				if( (ret = connect(sn, destip, destport)) != SOCK_OK) return ret; /*socket���ӷ�����*/ 
      break;
      case SOCK_CLOSED:														/*socket���ڹر�״̬*/
    	  close(sn);
    	  if((ret=socket(sn, Sn_MR_TCP, local_port, 0x00)) != sn) return ret; // TCP socket open with 'any_port' port number
         break;
      default:
         break;
   }
   return 1;
}

