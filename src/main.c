#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <util/delay.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "ioconfig.h"

#include "uart.h"

#include "main.h"

#define VERSION "0.0.1"

static void rts_init(void);

static void setLED(uint8_t status);
static void blinkLED(uint8_t times, uint8_t fast); // Blink the led X times either fast (50ms) or slow (100ms)
static void soft_reset(void);

int main(void) {
#if defined (__AVR_ATmega328P__)
    wdt_enable(WDTO_4S); // Enable the watchdog to reset in 4 seconds...
#elif defined (__AVR_ATmega8A__)
    wdt_enable(WDTO_2S); // Enable the watchdog to reset in 2 seconds...
#endif

    // Initialize the I/O and communications
    io_init();

    // First watchdog kick
    wdt_reset();

    // Initialize serial port
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

    // Enable interrupts
    sei();

    setLED(1); // Turn the LED on

    while(1) {
        wdt_reset(); // Kick the watchdog
    }

    return 0;
}

static void setLED(uint8_t status) {
    if(!status) LEDPORT &= ~(_BV(LED_P)); // Turn the LED off
    else LEDPORT |= _BV(LED_P); // Turn the LED on
}

static void blinkLED(uint8_t times, uint8_t fast) {
    setLED(0);

    while(times--) {
        wdt_reset();

        setLED(1);
        fast ? _delay_ms(50) : _delay_ms(100);
        setLED(0);
        fast ? _delay_ms(50) : _delay_ms(100);
    }
}

ISR(INT1_vect) { // Manage INT1
}

static void soft_reset(void) {
    wdt_enable(WDTO_15MS);  
    while(1); // This will reset the unit
}
