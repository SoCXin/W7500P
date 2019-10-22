#ifndef _TCPC_H_
#define _TCPC_H_

#include "wizchip_conf.h"
#include "W7500x.h"
#include <stdint.h>
#define	_LOOPBACK_DEBUG_/* Loopback test debug message printout enable */
#define DATA_BUF_SIZE			2048
/* TCP client Loopback test example */
int32_t tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport,uint16_t local_port);


#endif

