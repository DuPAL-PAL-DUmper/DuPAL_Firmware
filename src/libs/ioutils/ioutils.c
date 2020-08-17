#include "ioutils.h"

#include <ioutils/mcu_io.h>
#include <shifter/sipo_shifter.h>
#include <shifter/piso_shifter.h>

void ioutils_write(uint32_t val) {
    sipo_shifter_set(val);
}

uint8_t ioutils_read(void) {
    return piso_shifter_get();
}

void ioutils_setLED(uint8_t status) {
    if(!status) ACT_LEDPORT &= ~(_BV(ACT_LED_P)); // Turn the LED off
    else ACT_LEDPORT |= _BV(ACT_LED_P); // Turn the LED on
}