#include "ioutils.h"

#include <ioutils/mcu_io.h>
#include <shifter/shifter.h>

void ioutils_write(uint32_t val) {
    shifter_set(val);
}

void ioutils_setLED(uint8_t status) {
    if(!status) ACT_LEDPORT &= ~(_BV(ACT_LED_P)); // Turn the LED off
    else ACT_LEDPORT |= _BV(ACT_LED_P); // Turn the LED on
}