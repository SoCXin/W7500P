#ifndef _TCPS_H_
#define _TCPS_H_

#include <stdint.h>

/* Loopback test debug message printout enable */
#define	_LOOPBACK_DEBUG_

/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			2048
#endif

#define LOOPBACK_MAIN_NOBLOCK    0
#define LOOPBACK_MODE   LOOPBACK_MAIN_NOBLOCK

/* TCP server Loopback test example */
int32_t tcps(uint8_t sn, uint8_t* buf, uint16_t port);


#endif
