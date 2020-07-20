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

#define LEDPORT	 PORTB
#define LEDPIN   PINB
#define LEDDDR   DDRB
#define LED_P    5 // PB5

#define SHFT_PORT_1 PORTB
#define SHFT_PIN_1  PINB
#define SHFT_DDR_1  DDRB
#define SHFT_1_SER  0 // PB0
#define SHFT_1_CLK  1 // PB1
#define SHFT_1_RST  2 // PB2

#define SHFT_PORT_2 PORTC
#define SHFT_PIN_2  PINC
#define SHFT_DDR_2  DDRC
#define SHFT_2_RCLK 0 // PC0
#define SHFT_2_OE   1 // PC1

void io_init(void);

/*
 * This will return a byte that contains the status of the PAL pins, from MSB to LSB 
 * 12, 19, 13, 14, 15, 16, 17, 18
 */
uint8_t io_read(void);

/*
 * Write the two pins directly connected to the MCU, from MSB to LSB
 * 12, 19
 */
void io_write(uint8_t val);

#endif /* _MCU_IO_H_ */
