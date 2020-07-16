#include "ioconfig.h"

void io_init() {
    // Config the LED port
    LEDDDR |= _BV(LED_P); // Make LED port an output
    LEDPORT &= ~(_BV(LED_P)); // Turn the LED off

    // Config the shifter ports
    SHFT_DDR_1 |= (_BV(SHFT_1_SER) | _BV(SHFT_1_CLK) | _BV(SHFT_1_RST)); // Configure these pins as output
    SHFT_DDR_2 |= (_BV(SHFT_2_RCLK) | _BV(SHFT_2_OE));

    // Set the other ports for PAL communication as IN or OUT
    DDRC &= ~(_BV(2) | _BV(3) | _BV(4) | _BV(5)); // PC2-5 as inputs
    DDRD &= ~(_BV(2) | _BV(3) | _BV(4) | _BV(5)); // PD2-5 as inputs
    DDRD |= (_BV(6) | _BV(7)); // PD6,7 as outputs
}

