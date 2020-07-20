#include "PAL12L6.h"

#include <string.h>

#include <avr/wdt.h>
#include <util/delay.h>

#include <uart/uart.h>
#include <ioutils/mcu_io.h>
#include <ioutils/ioutils.h>
#include <utils/strutils.h>

static void print_pinstat(uint16_t idx, uint8_t input, uint8_t floating);

void pal12l6_analyze(void) {
    uart_puts("-[ PAL12L6 analyzer ]-\n\n");

    ioutils_setLED(1); // Turn the LED on

    uart_puts("        INPUTS          |   OUTPUTS  \n");
    uart_puts("0 0 0 0 0 0 0 0 0 1 1 1 | 1 1 1 1 1 1\n");
    uart_puts("1 2 3 4 5 6 7 8 9 1 2 9 | 8 7 6 5 4 3\n");
    uart_puts("-------------------------------------\n");

    uint8_t read_1, read_2, floating;
    for(uint32_t idx = 0; idx <= 0x0FFF; idx++) {
        wdt_reset();

        ioutils_setLED(1);

        uint32_t comp_idx = (idx & 0x3FF) | ((uint32_t)(idx & 0x0C00) << 6); // Set the address and pull the outputs low
        ioutils_write(comp_idx);
        _delay_us(50);
        read_1 = io_read() & 0x3F;

        comp_idx = (idx & 0x3FF) | ((uint32_t)(idx & 0x0C00) << 6) | 0xFC00; // Same address with the outputs high
        ioutils_write(comp_idx);
        _delay_us(50);
        read_2 = io_read() & 0x3F;

        floating = ((read_1 ^ read_2) & 0x3F);

        ioutils_setLED(0);

        print_pinstat(idx, read_2, floating);
    }
}

static void print_pinstat(uint16_t idx, uint8_t input, uint8_t floating) {
    memset(str_buf, 0, STR_BUF_SIZE);

    char *str_ptr = str_buf;
    for(uint8_t i = 0; i < 12; i++) {
        *str_ptr = ((idx >> i) & 0x01) ? '1' : '0';
        str_ptr++;
        *str_ptr = ' ';
        str_ptr++;
    }

    *str_ptr = '|';
    str_ptr++;
    *str_ptr = ' ';
    str_ptr++;

    for(uint8_t i = 0; i < 6; i++) {
        if((floating >> i) & 0x01) *str_ptr = 'x'; // Pin is floating... shouldn't happen here, probably broken
        else *str_ptr = ((input >> i) & 0x01) ? '1' : '0';
        str_ptr++;
        *str_ptr = ' ';
        str_ptr++;
    } 

    *str_ptr = '\n';

    uart_puts(str_buf);
}