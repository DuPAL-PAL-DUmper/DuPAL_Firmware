#include "shifter.h"

#include <util/delay.h>
#include <ioconfig.h>

static inline void toggle_SRCLK(void);
static inline void toggle_RCLK(void);

void shifter_init(void) {
    SHFT_PORT_2 |= _BV(SHFT_2_OE); // Disable the outputs (/OE high)
    SHFT_PORT_1 &= ~(_BV(SHFT_1_SER)); // Set SER low for now
    SHFT_PORT_2 &= ~(_BV(SHFT_2_RCLK)); // Set SRCLK low for now

    SHFT_PORT_1 &= ~(_BV(SHFT_1_RST)); // Reset the shift registers (/SRCLR low)
    _delay_ms(5);
    SHFT_PORT_1 |= _BV(SHFT_1_RST); // /SRCLR high
    
    SHFT_PORT_2 &= ~(_BV(SHFT_2_OE)); // Enable the outputs
}

void shifter_set(uint16_t val) {
    for(uint8_t i = 0; i < 16; i++) {
        if((val >> (15-i)) & 0x01) SHFT_PORT_1 |= _BV(SHFT_1_SER); // High
        else SHFT_PORT_1 &= ~(_BV(SHFT_1_SER)); // Low

        toggle_SRCLK();
    }

    toggle_RCLK(); // Send out the values 
}

static inline void toggle_SRCLK(void) {
    SHFT_PORT_1 |= _BV(SHFT_1_CLK); // Set CLK high
    _delay_us(100);
    SHFT_PORT_1 &= ~(_BV(SHFT_1_CLK)); // Set CLK low
}

static inline void toggle_RCLK(void) {
    SHFT_PORT_2 |= _BV(SHFT_2_RCLK); // Set RCLK high
    _delay_us(100);
    SHFT_PORT_2 &= ~(_BV(SHFT_2_RCLK)); // Set RCLK low
}