#ifndef __USART_H__
#define __USART_H__

void uart3_init(void);

char uart3_getc(void);

void uart3_putc(char data);

void uart3_puts(char *string);

#endif
