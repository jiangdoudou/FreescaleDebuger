#ifndef _PRINTF_H
#define _PRINTF_H

#include "headers.h"

unsigned char uart_getkey(void);
void uart_init(void);
void uart_putchar(unsigned char ch);
void putstr(char ch[]);
void printp(char *, ...);

#endif