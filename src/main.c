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

#include "main.h"

#define VERSION "0.0.3"
#define SOFT_HEADER "\nDuPAL - " VERSION "\n\n"

#define STR_BUF_SIZE 128
static char str_buf[STR_BUF_SIZE];

static void setLED(uint8_t status);
static void format_ioconf(uint8_t inputs);
static void format_brutef(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status, char special_symbol);

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

    uart_puts("Detecting inputs...\n");

    // Detect the inputs on the PAL
   uint8_t io_inputs = detect_inputs();

    if(io_inputs == 0x3F) { // All IOs as input? maybe there is no chip inserted
        uart_puts("WARNING: All the IOs are floating... Maybe no or broken chip inserted?\n\n");
    }

    wdt_reset();

    format_ioconf(io_inputs);
    
    // Reset the watchdog and blink a bit
    for(uint8_t i = 0; i < 5; i++) {
        setLED(1);
        _delay_ms(500);
        setLED(0);
        _delay_ms(500);
        wdt_reset();
    }

    uart_puts("           INPUTS               |     OUTPUTS    \n");
    uart_puts("0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 | 1 1 1 1 1 1 1 1\n");
    uart_puts("1 2 3 4 5 6 7 8 9 1 8 7 6 5 4 3 | 8 7 6 5 4 3 9 2\n");
    uart_puts("-------------------------------------------------\n");

    uint8_t read_1, read_2, floating;
    // At worst, if all the IOs are set as inputs, we'll have to try 65536 combinations!
    for(uint32_t idx = 0; idx <= 0xFFFF; idx++) {
        wdt_reset(); // Kick the watchdog

        if((idx >> 10) & (~io_inputs & 0x3F)) continue; // Skip this round

        setLED(1);
        // First, try to force the the IOs and outputs to low
        compound_io_write(idx);
        _delay_us(50);
        read_1 = io_read();
        
        // Then try to force them to high
        // We will also try to force the pins that we did not detect as input in the beginning
        compound_io_write(0x030000 | (((uint32_t)(~io_inputs & 0x3F)) << 10) | idx);
        _delay_us(50);
        read_2 = io_read();

        floating = (read_1 ^ read_2);

        setLED(0);

        format_brutef(idx, io_inputs, floating, read_2, ' ');

        // If we get in here, we're in a situation where we found some IOs what we detected as outputs
        // that have become high-impedence. 
        // In this case, they could be read as inputs and change the state of the other outputs,
        // so we treat them as inputs and try to toggle all the combinations out of them to see if anything changes.
        if(floating & 0x3F) {
            for(uint8_t io_idx = 0; io_idx < 0x3F; io_idx++) { // Try all the combinations of 6 bits
                wdt_reset();

                if((io_idx & floating) != io_idx) continue; // Skip this run, we would be toggling pins that we did not find floating
                compound_io_write((((uint32_t)(io_idx & 0x3F)) << 10) | idx); // Toggle them, by writing the current indices plus the combination of IO pins we found floating
                read_1 = io_read(); // Read the result
                
                // Print what we read, but make sure to
                // 1. Update the index with the current combination of outputs
                // 2. Update the input list by adding the floating IOs we're toggling
                // 3. Remove from the floating list the ones we're toggling
                format_brutef(((((uint32_t)(io_idx & 0x3F)) << 10) | idx), (io_inputs | (floating & 0x3F)), (floating & 0xC0), read_1, '<');
            }
        } 
    }

    uart_puts("Dump complete.\n");

    // We're done, blink the led at 1Hz
    while(1) {
        _delay_ms(1000);
        wdt_reset(); // Kick the watchdog
        setLED(0);
        _delay_ms(1000);
        wdt_reset(); // Kick the watchdog
    }


    return 0;
}

static void setLED(uint8_t status) {
    if(!status) LEDPORT &= ~(_BV(LED_P)); // Turn the LED off
    else LEDPORT |= _BV(LED_P); // Turn the LED on
}

static uint8_t detect_inputs(void) {
    uint8_t read1, read2;
    uint8_t inputs = 0xFF;

    setLED(1);

    for(uint16_t idx = 0; idx < 0x3FF; idx++) {
        compound_io_write(idx); // Zero the potential outputs
        _delay_us(50);
        read1 = io_read();
        compound_io_write(0xFC00 | idx); // Pull high all the IOx pins on the PAL, the rest of the address will remain the same
        _delay_us(50); 
        read2 = io_read();

        inputs &= ((read1 ^ read2) & 0x3F);

        wdt_reset();
    }
    
    setLED(0);

    return inputs;
}

static void format_ioconf(uint8_t inputs) {
    memset(str_buf, 0, STR_BUF_SIZE);
    sprintf(str_buf, "Detected IO config:\n");
    uart_puts(str_buf);
    
    uart_puts("IO1 IO2 IO3 IO4 IO5 IO6\n");
    memset(str_buf, 0, STR_BUF_SIZE);
    sprintf(str_buf, " %c   %c   %c   %c   %c   %c\n\n", 
    ((inputs >> 0) & 0x01) ? 'I' : 'O',
    ((inputs >> 1) & 0x01) ? 'I' : 'O',
    ((inputs >> 2) & 0x01) ? 'I' : 'O',
    ((inputs >> 3) & 0x01) ? 'I' : 'O',
    ((inputs >> 4) & 0x01) ? 'I' : 'O',
    ((inputs >> 5) & 0x01) ? 'I' : 'O');
    uart_puts(str_buf);
}

static void format_brutef(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status, char special_symbol) {
    memset(str_buf, 0, STR_BUF_SIZE);

    char *str_ptr = str_buf;
    for(uint8_t i = 0; i < 10; i++) {
        *str_ptr = ((idx >> i) & 0x01) ? '1' : '0';
        str_ptr++;
        *str_ptr = ' ';
        str_ptr++;
    }

    for(uint8_t i = 0; i < 6; i++) {
        if(((inputs >> i) & 0x01)) *str_ptr = ((idx >> (10+i)) & 0x01) ? '1' : '0';
        else *str_ptr = '.';

        str_ptr++;
        *str_ptr = ' ';
        str_ptr++;
    }

    *str_ptr = '|';
    str_ptr++;
    *str_ptr = ' ';
    str_ptr++;

    for(uint8_t i = 0; i < 6; i++) {
        if((floating >> i) & 0x01) *str_ptr = 'x'; // Check that this output is not in high impedence mode
        else if(!(inputs >> i & 0x01)) *str_ptr = ((out_status >> i) & 0x01) ? '1' : '0';
        else *str_ptr = '.';
        str_ptr++;
        *str_ptr = ' ';
        str_ptr++;
    }

    if(floating & 0x40) *str_ptr = 'x';
    else *str_ptr = ((out_status >> 6) & 0x01) ? '1' : '0';
    str_ptr++;
    *str_ptr = ' ';
    str_ptr++;

    if(floating & 0x80) *str_ptr = 'x';
    else *str_ptr = ((out_status >> 7) & 0x01) ? '1' : '0';
    str_ptr++;

    // Print the special symbol
    *str_ptr = ' ';
    str_ptr++;
    *str_ptr = special_symbol;
    str_ptr++;

    *str_ptr = '\n';

    uart_puts(str_buf);
}

ISR(INT1_vect) { // Manage INT1
}