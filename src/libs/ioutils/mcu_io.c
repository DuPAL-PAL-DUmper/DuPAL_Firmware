#include "mcu_io.h"

void io_init() {
    // Config the Activity LED port
    ACT_LEDDDR |= _BV(ACT_LED_P); // Make LED port an output
    ACT_LEDPORT &= ~(_BV(ACT_LED_P)); // Turn the LED off
    
    // Config the Socket LED port
    SOCK_LEDDDR |= (_BV(SOCK_LED_P1) | _BV(SOCK_LED_P2));
    SOCK_LEDPORT &= ~(_BV(SOCK_LED_P1) | _BV(SOCK_LED_P2)); // Turn the LEDs off

    // Config the SIPO shifter ports
    SIPO_DDR_1 |= (_BV(SIPO_1_SER) | _BV(SIPO_1_CLK) | _BV(SIPO_1_RST)); // Configure these pins as output
    SIPO_DDR_2 |= (_BV(SIPO_2_RCLK) | _BV(SIPO_2_OE));
    

    // Config the PISO shifter ports
    PISO_DDR |= (_BV(PISO_CLK) | _BV(PISO_CLR) | _BV(PISO_INH) | _BV(PISO_SH)); // Outputs
    PISO_DDR  &= ~_BV(PISO_SER); // Inputs
    PISO_PORT &= ~_BV(PISO_SER); // Disable pullup on input

    // Set the other ports for PAL communication as IN or OUT
    DDRC &= ~(_BV(2) | _BV(3)); // PC2,3 as inputs
    DDRD &= ~_BV(7); // PD7 as input
    PORTC |= (_BV(2) | _BV(3)); // Enable PC2-5 internal pull-ups
    PORTD |= _BV(7); // Enable PD7 internal pull-up
}
