#ifndef _IOCONFIG_H_
#define _IOCONFIG_H_

// I/O port definitions

#include <avr/io.h>

#define LEDPORT	 PORTB
#define LEDPIN   PINB
#define LEDDDR   DDRB
#define LED_P    5

void io_init(void);

#endif /* _IOCONFIG_H_ */
