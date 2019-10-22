#ifndef _UTILITY_H
#define _UTILITY_H
#include "W7500x.h"
#include"stdio.h"

uint16_t ATOI(char* str,uint16_t base); 			/* Convert a string to integer number */
uint32_t ATOI32(char* str,uint16_t base); 			/* Convert a string to integer number */
void itoa(uint16_t n,uint8_t* str, uint8_t len);
int ValidATOI(char* str, int base, int* ret); 		/* Verify character string and Convert it to (hexa-)decimal. */
char C2D(uint8_t c); 					/* Convert a character to HEX */

uint16_t swaps(uint16_t i);
uint32_t swapl(uint32_t l);

char VerifyIPAddress(char* src, uint8_t * ip);
char VerifyIPAddress_orig(char* src	);

void replacetochar(char * str, char oldchar, char newchar);

void mid(char* src, char* s1, char* s2, char* sub);
void inet_addr_(unsigned char* addr,unsigned char *ip);

char* inet_ntoa(unsigned long addr);			/* Convert 32bit Address into Dotted Decimal Format */
char* inet_ntoa_pad(unsigned long addr);

unsigned long inet_addr(unsigned char* addr);		/* Converts a string containing an (Ipv4) Internet Protocol decimal dotted address into a 32bit address */

#endif
