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
    // TODO

    // Set the other ports for PAL communication as IN or OUT
    DDRC &= ~(_BV(2) | _BV(3) | _BV(4) | _BV(5)); // PC2-5 as inputs
    DDRD &= ~(_BV(2) | _BV(3) | _BV(4) | _BV(5)); // PD2-5 as inputs
    PORTC &= ~(_BV(2) | _BV(3) | _BV(4) | _BV(5)); // Disable PC2-5 internal pull-ups
    PORTD &= ~(_BV(2) | _BV(3) | _BV(4) | _BV(5)); // Disable PD2-5 internal pull-ups
    DDRD |= (_BV(6) | _BV(7)); // PD6,7 as outputs
}

uint8_t io_read(void) {
    return ((PINC & (_BV(2) | _BV(3) | _BV(4) | _BV(5))) >> 2) |
            ((PIND & (_BV(2) | _BV(3) | _BV(4) | _BV(5))) << 2);
}

void io_write(uint8_t val) {
    PORTD = (PORTD & ~(_BV(6) | _BV(7))) | (val & 0x03) << 6; 
}

