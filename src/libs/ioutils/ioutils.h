#ifndef _IOUTILS_H_
#define _IOUTILS_H_

#include <stdint.h>

typedef enum {
    ACT_LED = 0,
    P20_LED = 1,
    P24_LED = 2
} led;

/*
 * This function will use both shifters and MCU to toggle 18 outputs 
 * From MSB to LSB, the following pins on the PAL socket are toggled
 * 
 * 24 - pin | 20 - pin
 * ---------+---------
 * 23       | xx 
 * 14       | xx
 * 13       | xx
 * 11       | xx
 * 22       | 12       (resistor)
 * 21       | 19       (resistor)
 * 20       | 13       (resistor)
 * 19       | 14       (resistor)
 * 18       | 15       (resistor)
 * 17       | 16       (resistor)
 * 16       | 17       (resistor)
 * 15       | 18       (resistor)
 * 10       | 11
 *  9       |  9
 *  8       |  8
 *  7       |  7
 *  6       |  6
 *  5       |  5
 *  4       |  4
 *  3       |  3
 *  2       |  2
 *  1       |  1
 */
void ioutils_write(uint32_t val);

/*
 * This will return a byte that contains the status of the PAL pins, from MSB to LSB 
 * 
 * 24 - pin | 20 - pin
 * ---------+---------
 * 12       | 22
 * 19       | 21
 * 13       | 20
 * 14       | 19
 * 15       | 18
 * 16       | 17
 * 17       | 16
 * 18       | 15
 */
uint8_t ioutils_read(void);

void ioutils_setLED(led l, uint8_t status);

#endif /* _IOUTILS_H_ */