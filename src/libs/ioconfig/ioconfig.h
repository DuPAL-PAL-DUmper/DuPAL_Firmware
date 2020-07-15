#ifndef _IOCONFIG_H_
#define _IOCONFIG_H_

// I/O port definitions

#include <avr/io.h>

#define LEDPORT	 PORTB
#define LEDPIN   PINB
#define LEDDDR   DDRB
#define LED_P    5 // PB5

#define SHFT_PORT_1 PORTB
#define SHFT_PIN_1  PINB
#define SHFT_DDR_1  DDRB
#define SHFT_1_SER  0 // PB0
#define SHFT_1_CLK  1 // PB1
#define SHGT_1_RST  2 // PB2

#define SHFT_PORT_2 PORTC
#define SHFT_PIN_2  PINC
#define SHFT_DDR_2  DDRC
#define SHFT_2_RCLK 0 // PC0
#define SHFT_2_OE   1 // PC1

void io_init(void);

#endif /* _IOCONFIG_H_ */
