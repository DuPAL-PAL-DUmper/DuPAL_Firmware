#ifndef _MCU_IO_H_
#define _MCU_IO_H_

/*  
 *  Connections map
 *
 *  PB0 -> '595 SER    (OUT)
 *  PB1 -> '595 CLK    (OUT)
 *  PB2 -> '595 /RESET (OUT)  
 *  PC0 -> '595 RCLK   (OUT)
 *  PC1 -> '595 /OE    (OUT)
 * 
 *  PC2 -> PAL IO1 direct (IN)
 *  PC3 -> PAL IO2 direct (IN)
 *  PC4 -> PAL IO3 direct (IN)
 *  PC5 -> PAL IO4 direct (IN)
 *  PD2 -> PAL IO5 direct (IN)
 *  PD3 -> PAL IO6 direct (IN)
 * 
 *  PD4 -> PAL TRIO1 direct (IN)
 *  PD5 -> PAL TRIO2 direct (IN)
 * 
 *  PD6 -> PAL TRIO1 resistor (OUT)
 *  PD7 -> PAL TRIO2 resistor (OUT)
 * 
 *  PB5 -> Activity LED (OUT)
 * 
 */


// I/O port definitions
#include <avr/io.h>
#include <stdint.h>

#define ACT_LEDPORT	 PORTB
#define ACT_LEDPIN   PINB
#define ACT_LEDDDR   DDRB
#define ACT_LED_P    5 // PB5 (out)

#define SOCK_LEDPORT PORTC
#define SOCK_LEDPINC PINC
#define SOCK_LEDDDR  DDRC
#define SOCK_LED_P1  4 // PC5 (20-pin PAL, out)
#define SOCK_LED_P2  5 // PC6 (24-pin PAL, out)

#define PISO_PORT  PORTD
#define PISO_PIN   PIND
#define PISO_DDR   DDRD
#define PISO_CLR   2 // PD2 (out)
#define PISO_SH    3 // PD3 (out)
#define PISO_INH   4 // PD4 (out)
#define PISO_CLK   5 // PD5 (out)
#define PISO_SER   6 // PD6 (in)

#define SIPO_PORT_1 PORTB
#define SIPO_PIN_1  PINB
#define SIPO_DDR_1  DDRB
#define SIPO_1_SER  0 // PB0 (out)
#define SIPO_1_CLK  1 // PB1 (out)
#define SIPO_1_RST  2 // PB2 (out)

#define SIPO_PORT_2 PORTC
#define SIPO_PIN_2  PINC
#define SIPO_DDR_2  DDRC
#define SIPO_2_RCLK 0 // PC0
#define SIPO_2_OE   1 // PC1

void io_init(void);

#endif /* _MCU_IO_H_ */
