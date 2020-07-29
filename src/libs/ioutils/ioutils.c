#include "ioutils.h"

#include <ioutils/mcu_io.h>
#include <shifter/shifter.h>

void ioutils_write(uint32_t val) {
    io_write((uint8_t)((val >> 16) & 0x03));
    shifter_set((uint16_t)(val & 0xFFFF));
}

void ioutils_setLED(uint8_t status) {
    if(!status) LEDPORT &= ~(_BV(LED_P)); // Turn the LED off
    else LEDPORT |= _BV(LED_P); // Turn the LED on
}