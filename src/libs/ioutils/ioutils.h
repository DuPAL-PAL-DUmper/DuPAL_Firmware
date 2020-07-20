#ifndef _IOUTILS_H_
#define _IOUTILS_H_

#include <stdint.h>

/*
 * This function will use both shifters and MCU to toggle 18 outputs 
 * From MSB to LSB, the following pins on the PAL socket are toggled
 * 12 (resistor)
 * 19 (resistor)
 * 13 (resistor)
 * 14 (resistor)
 * 15 (resistor)
 * 16 (resistor)
 * 17 (resistor)
 * 18 (resistor)
 * 11
 *  9
 *  8
 *  7
 *  6
 *  5
 *  4
 *  3
 *  2
 *  1
 */
void ioutils_write(uint32_t val);

#endif /* _IOUTILS_H_ */