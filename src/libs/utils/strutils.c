#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strutils.h"
#include <uart/uart.h>

#define STR_BUF_SIZE 128
static char str_buf[STR_BUF_SIZE];

void strutils_print_ioconf(uint8_t inputs) {
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

void strutils_print_pinstat(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status, char special_symbol) {
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