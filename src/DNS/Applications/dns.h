#ifndef	_DNS_H_
#define	_DNS_H_

#include <stdint.h>

#define	MAX_DNS_BUF_SIZE	256		///< maximum size of DNS buffer. */
#define  MAX_DOMAIN_NAME   16       // for example "www.google.com"
#define	MAX_DNS_RETRY     2        ///< Requery Count
#define	DNS_WAIT_TIME     3        ///< Wait response time. unit 1s.
#define	IPPORT_DOMAIN     53       ///< DNS server port number
#define DNS_MSG_ID         0x1122   ///< ID for DNS message. You can be modifyed it any number

#define	INITRTT		2000L	/* Initial smoothed response time */
#define	MAXCNAME	   (MAX_DOMAIN_NAME + (MAX_DOMAIN_NAME>>1))	   /* Maximum amount of cname recursion */

#define	TYPE_A		1	   /* Host address */
#define	TYPE_NS		2	   /* Name server */
#define	TYPE_MD		3	   /* Mail destination (obsolete) */
#define	TYPE_MF		4	   /* Mail forwarder (obsolete) */
#define	TYPE_CNAME	5	   /* Canonical name */
#define	TYPE_SOA	   6	   /* Start of Authority */
#define	TYPE_MB		7	   /* Mailbox name (experimental) */
#define	TYPE_MG		8	   /* Mail group member (experimental) */
#define	TYPE_MR		9	   /* Mail rename name (experimental) */
#define	TYPE_NULL	10	   /* Null (experimental) */
#define	TYPE_WKS	   11	   /* Well-known sockets */
#define	TYPE_PTR	   12	   /* Pointer record */
#define	TYPE_HINFO	13	   /* Host information */
#define	TYPE_MINFO	14	   /* Mailbox information (experimental)*/
#define	TYPE_MX		15	   /* Mail exchanger */
#define	TYPE_TXT	   16	   /* Text strings */
#define	TYPE_ANY	   255	/* Matches any type */

#define	CLASS_IN	   1	   /* The ARPA Internet */

/* Round trip timing parameters */
#define	AGAIN	      8     /* Average RTT gain = 1/8 */
#define	LAGAIN      3     /* Log2(AGAIN) */
#define	DGAIN       4     /* Mean deviation gain = 1/4 */
#define	LDGAIN      2     /* log2(DGAIN) */

/* Header for all domain messages */
struct dhdr
{
	uint16_t id;   /* Identification */
	uint8_t	qr;      /* Query/Response */
#define	QUERY    0
#define	RESPONSE 1
	uint8_t	opcode;
#define	IQUERY   1
	uint8_t	aa;      /* Authoratative answer */
	uint8_t	tc;      /* Truncation */
	uint8_t	rd;      /* Recursion desired */
	uint8_t	ra;      /* Recursion available */
	uint8_t	rcode;   /* Response code */
#define	NO_ERROR       0
#define	FORMAT_ERROR   1
#define	SERVER_FAIL    2
#define	NAME_ERROR     3
#define	NOT_IMPL       4
#define	REFUSED        5
	uint16_t qdcount;	/* Question count */
	uint16_t ancount;	/* Answer count */
	uint16_t nscount;	/* Authority (name server) count */
	uint16_t arcount;	/* Additional record count */
};

void DNS_init(uint8_t s, uint8_t * buf);

int8_t DNS_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns);

void DNS_time_handler(void);

#endif	/* _DNS_H_ */
