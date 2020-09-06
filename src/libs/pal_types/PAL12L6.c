#include "PAL12L6.h"

#include <string.h>

#include <avr/wdt.h>
#include <util/delay.h>

#include <uart/uart.h>
#include <ioutils/ioutils.h>
#include <utils/strutils.h>

static void print_pinstat(uint16_t idx, uint8_t input, uint8_t floating);

void pal12l6_analyze(void) {
    uart_puts("-[ PAL12L6 analyzer ]-\n\n");

    ioutils_setLED(ACT_LED, 1); // Turn the Activity LED on
    ioutils_setLED(P20_LED, 1); // Turn P20 LED on

    uart_puts(MARKER_STRING);
    uart_puts(".i 12\n");
    uart_puts(".o 6\n");
    uart_puts(".ilb i1 i2 i3 i4 i5 i6 i7 i8 i9 i11 i12 i19\n");
    uart_puts(".ob o18 o17 o16 o15 o14 o13\n");
    uart_puts(".phase 000000\n");

    uint8_t read_1, read_2, floating;
    for(uint32_t idx = 0; idx <= 0x0FFF; idx++) {
        wdt_reset();

        ioutils_setLED(ACT_LED, 1); // Activity LED

        uint32_t comp_idx = (idx & 0x3FF) | ((uint32_t)(idx & 0x0C00) << 6); // Set the address and pull the outputs low
        ioutils_write(comp_idx);
        _delay_us(50);
        read_1 = ioutils_read() & 0x3F;

        comp_idx = (idx & 0x3FF) | ((uint32_t)(idx & 0x0C00) << 6) | 0xFC00; // Same address with the outputs high
        ioutils_write(comp_idx);
        _delay_us(50);
        read_2 = ioutils_read() & 0x3F;

        floating = ((read_1 ^ read_2) & 0x3F);

        ioutils_setLED(ACT_LED, 0);

        print_pinstat(idx, read_2, floating);
    }

    uart_puts(".e\n");
    uart_puts(MARKER_STRING);
    
    ioutils_setLED(P20_LED, 0); // Turn P20 LED off
}

static void print_pinstat(uint16_t idx, uint8_t input, uint8_t floating) {
    memset(str_buf, 0, STR_BUF_SIZE);

    char *str_ptr = str_buf;
    // Print the inputs
    for(uint8_t i = 0; i < 12; i++) {
        *str_ptr = ((idx >> i) & 0x01) ? '1' : '0';
        str_ptr++;
    }

    *str_ptr = ' ';
    str_ptr++;

    // Print the outputs
    for(uint8_t i = 0; i < 6; i++) {
        if((floating >> i) & 0x01) *str_ptr = '-'; // Pin is floating... shouldn't happen here, probably broken
        else *str_ptr = ((input >> i) & 0x01) ? '1' : '0';
        str_ptr++;
    } 

    *str_ptr = '\n';

    uart_puts(str_buf);
}