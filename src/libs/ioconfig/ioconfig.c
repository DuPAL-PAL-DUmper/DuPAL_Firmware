#include "ioconfig.h"

void io_init() {
    LEDDDR |= _BV(LED_P); // Make LED port an output
    LEDPORT &= ~(_BV(LED_P)); // Turn the LED off
}

