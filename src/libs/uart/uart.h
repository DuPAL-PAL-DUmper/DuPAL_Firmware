#ifndef _UART_HEADER_
#define _UART_HEADER_

int uart_putchar(char c);
int uart_getchar(void);

void uart_init(void);

void uart_puts(char *c);

#endif
