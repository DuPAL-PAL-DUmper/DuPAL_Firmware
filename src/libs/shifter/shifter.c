#include "shifter.h"

#include <util/delay.h>
#include <mcu_io.h>

static inline void toggle_SRCLK(void);
static inline void toggle_RCLK(void);

void shifter_init(void) {
    SIPO_PORT_2 |= _BV(SIPO_2_OE); // Disable the outputs (/OE high)
    SIPO_PORT_1 &= ~(_BV(SIPO_1_SER)); // Set SER low for now
    SIPO_PORT_2 &= ~(_BV(SIPO_2_RCLK)); // Set SRCLK low for now

    SIPO_PORT_1 &= ~(_BV(SIPO_1_RST)); // Reset the shift registers (/SRCLR low)
    _delay_ms(5);
    SIPO_PORT_1 |= _BV(SIPO_1_RST); // /SRCLR high
    
    SIPO_PORT_2 &= ~(_BV(SIPO_2_OE)); // Enable the outputs
}

void shifter_set(uint32_t val) {
    for(uint8_t i = 0; i < 24; i++) {
        if((val >> i) & 0x01) SIPO_PORT_1 |= _BV(SIPO_1_SER); // High
        else SIPO_PORT_1 &= ~(_BV(SIPO_1_SER)); // Low

        toggle_SRCLK();
    }

    toggle_RCLK(); // Send out the values 
}

static inline void toggle_SRCLK(void) {
    SIPO_PORT_1 |= _BV(SIPO_1_CLK); // Set CLK high
    _delay_us(100);
    SIPO_PORT_1 &= ~(_BV(SIPO_1_CLK)); // Set CLK low
}

static inline void toggle_RCLK(void) {
    SIPO_PORT_2 |= _BV(SIPO_2_RCLK); // Set RCLK high
    _delay_us(100);
    SIPO_PORT_2 &= ~(_BV(SIPO_2_RCLK)); // Set RCLK low
}