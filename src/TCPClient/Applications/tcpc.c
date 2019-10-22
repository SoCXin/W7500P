/**
******************************************************************************
* @file   		tcpc.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-xx-xx
* @brief   		TCP 演示函数
* @attention  
******************************************************************************
**/
#include <stdio.h>
#include "tcpc.h"
#include "socket.h"
#include "wizchip_conf.h"

/**
*@brief		TCP Client回环演示函数。
*@param		无
*@return	无
*/
int32_t tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport,uint16_t local_port)
{
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0, sentsize=0;
   switch(getSn_SR(sn))														/*获取socket的状态*/
   {
			case SOCK_ESTABLISHED : 										/*socket处于连接建立状态*/
					if(getSn_IR(sn) & Sn_IR_CON)	
					{
						setSn_IR(sn, Sn_IR_CON);   						/*清除接收中断标志位*/
					}
					if((size = getSn_RX_RSR(sn)) > 0) 
					{
						if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
						ret = recv(sn, buf, size);						/*接收来自Server的数据*/		
						if(ret <= 0) return ret;
						sentsize = 0;
						while(size != sentsize)
						{
							ret = send(sn, buf+sentsize, size-sentsize);/*向Server发送数据*/
							if(ret < 0) 
							{
								close(sn); 
								return ret;
							}
							sentsize += ret;
						}
					}
      break;
      case SOCK_CLOSE_WAIT : 											/*socket处于等待关闭状态*/
         if((ret=disconnect(sn)) != SOCK_OK) return ret;
					#ifdef _LOOPBACK_DEBUG_
							printf("%d:Socket Closed\r\n", sn);
					#endif
      break;
      case SOCK_INIT :												/*socket处于初始化状态*/
				#ifdef _LOOPBACK_DEBUG_
						printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
				#endif
				if( (ret = connect(sn, destip, destport)) != SOCK_OK) return ret; /*socket连接服务器*/ 
      break;
      case SOCK_CLOSED:														/*socket处于关闭状态*/
    	  close(sn);
    	  if((ret=socket(sn, Sn_MR_TCP, local_port, 0x00)) != sn) return ret; // TCP socket open with 'any_port' port number
         break;
      default:
         break;
   }
   return 1;
}

