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

void ioutils_setLED(led l, uint8_t status) {
    switch(l) {
        case P20_LED:
            if(!status) SOCK_LEDPORT &= ~(_BV(SOCK_LED_P1)); // Turn the LED off
            else SOCK_LEDPORT |= _BV(SOCK_LED_P1); // Turn the LED on
            break;
        case P24_LED:
            if(!status) SOCK_LEDPORT &= ~(_BV(SOCK_LED_P2));
            else SOCK_LEDPORT |= _BV(SOCK_LED_P2); 
            break;
        case ACT_LED:
            if(!status) ACT_LEDPORT &= ~(_BV(ACT_LED_P));
            else ACT_LEDPORT |= _BV(ACT_LED_P);
            break;
        default:
            break;
    }
}