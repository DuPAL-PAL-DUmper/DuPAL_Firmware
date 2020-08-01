#include "PAL16L8.h"

#include <string.h>
#include <stdio.h>

#include <avr/wdt.h>
#include <util/delay.h>

#include <uart/uart.h>
#include <ioutils/mcu_io.h>
#include <ioutils/ioutils.h>
#include <utils/strutils.h>

static uint8_t detect_inputs(void);
static void print_6io_conf(uint8_t inputs);
static void print_pinstat(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status, char special_symbol);

void pal16l8_analyze(void) {
    uart_puts("-[ PAL16L8 / PAL10L8 analyzer ]-\n");

    ioutils_setLED(1); // Turn the LED on

    uart_puts("Detecting inputs...\n");

    // Detect the inputs on the PAL
    uint8_t io_inputs = detect_inputs();

    if(io_inputs == 0x3F) { // All IOs as input? maybe there is no chip inserted
        uart_puts("WARNING: All the IOs are floating... Maybe no or broken chip inserted?\n\n");
    }

    wdt_reset();

    print_6io_conf(io_inputs);

    // Reset the watchdog and blink a bit
    for(uint8_t i = 0; i < 5; i++) {
        ioutils_setLED(1);
        _delay_ms(500);
        ioutils_setLED(0);
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

        ioutils_setLED(1);
        // First, try to force the the IOs and outputs to low
        ioutils_write(idx);
        _delay_us(50);
        read_1 = io_read();

        // Then try to force them to high
        // We will also try to force the pins that we did not detect as input in the beginning
        ioutils_write(0x030000 | (((uint32_t)(~io_inputs & 0x3F)) << 10) | idx);
        _delay_us(50);
        read_2 = io_read();

        floating = (read_1 ^ read_2);

        ioutils_setLED(0);

        print_pinstat(idx, io_inputs, floating, read_2, ' ');
    }
}

static uint8_t detect_inputs(void) {
    uint8_t read1, read2;
    uint8_t inputs = 0xFF;

    ioutils_setLED(1);

    for(uint16_t idx = 0; idx < 0x3FF; idx++) {
        ioutils_write(idx); // Zero the potential outputs
        _delay_us(50);
        read1 = io_read();
        ioutils_write(0xFC00 | idx); // Pull high all the IOx pins on the PAL, the rest of the address will remain the same
        _delay_us(50);
        read2 = io_read();

        inputs &= ((read1 ^ read2) & 0x3F);

        wdt_reset();
    }

    ioutils_setLED(0);

    return inputs;
}

static void print_6io_conf(uint8_t inputs) {
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

static void print_pinstat(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status, char special_symbol) {
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

    // Pin 19
    if(floating & 0x40) *str_ptr = 'x';
    else *str_ptr = ((out_status >> 6) & 0x01) ? '1' : '0';
    str_ptr++;
    *str_ptr = ' ';
    str_ptr++;

    // Pin 12
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