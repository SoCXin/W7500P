#include <stdio.h>
#include "tcps.h"
#include "socket.h"
#include "wizchip_conf.h"


int32_t tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t ret;
   uint16_t size = 0, sentsize=0;

#ifdef _LOOPBACK_DEBUG_
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
					if(getSn_IR(sn) & Sn_IR_CON)
					{
						setSn_IR(sn,Sn_IR_CON);
					}
					if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
						if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
						ret = recv(sn, buf, size);

						if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
						sentsize = 0;
						while(size != sentsize)
						{
							ret = send(sn, buf+sentsize, size-sentsize);
							if(ret < 0)
							{
								close(sn);
								return ret;
							}
							sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
						}
         }
      break;
      case SOCK_CLOSE_WAIT :
					if((ret = disconnect(sn)) != SOCK_OK) return ret;
					#ifdef _LOOPBACK_DEBUG_
							printf("%d:Socket Closed\r\n", sn);
					#endif
      break;
      case SOCK_INIT :
					#ifdef _LOOPBACK_DEBUG_
							printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
					#endif
					if( (ret = listen(sn)) != SOCK_OK) return ret;
      break;
      case SOCK_CLOSED:
					if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
      break;
      default:
      break;
   }
   return 1;
}

