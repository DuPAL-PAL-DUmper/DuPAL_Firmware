#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/delay.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "ioconfig.h"

#include "uart.h"
#include "shifter.h"
#include "compound_io.h"
#include "strutils.h"

#include "main.h"

#define VERSION "0.0.1"
#define SOFT_HEADER "DuPAL - " VERSION "\n\n"

static volatile uint8_t io_status = 0x00; // This will reflect the status of pin 12-19 of the DuPAL
static volatile uint8_t io_inputs = 0x00; // From LSB to MSB, each bit will represent the input state of IO1 to IO6. If set to 1, it means the pin is an input
static volatile uint8_t trio_floating = 0x00; // From LSB to MSB, each bit will represent the floating state of TRIO1 and TRIO2. If set to 1, it means the pin is floating

static void rts_init(void);

static void setLED(uint8_t status);
static void blinkLED(uint8_t times, uint8_t fast); // Blink the led X times either fast (50ms) or slow (100ms)
static void soft_reset(void);

static uint8_t detect_inputs(void);

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

    // Prepare the shifter
    shifter_init();

    setLED(1); // Turn the LED on

    uart_puts(SOFT_HEADER); // Print the header

    // Detect the inputs on the PAL
    io_inputs = detect_inputs();

    if(io_inputs == 0x3F) { // All IOs as input? maybe there is no chip inserted
        uart_puts("WARNING: All the IOs are floating... Maybe no or broken chip inserted?\n\n");
    }

    //sprintf(str_buf, "Ins %.2X \n", io_inputs);
    //uart_puts(str_buf);

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

static uint8_t detect_inputs(void) {
    uint8_t read1, read2;
    compound_io_write(0); // Zero everything
    _delay_us(50);

    read1 = io_read();
    compound_io_write(0xFC00); // Pull high all the IOx pins on the PAL, the rest of the address will remain the same
    _delay_us(50);
    read2 = io_read();

    return (read1 ^ read2) & 0x3F;
}

ISR(INT1_vect) { // Manage INT1
}

static void soft_reset(void) {
    wdt_enable(WDTO_15MS);  
    while(1); // This will reset the unit
}