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

#define VERSION "0.0.1"
#define SOFT_HEADER "DuPAL - " VERSION "\n\n"

#define STR_BUF_SIZE 128
static char str_buf[STR_BUF_SIZE];

static void setLED(uint8_t status);
static void format_ioconf(uint8_t inputs);
static void format_brutef(uint16_t idx, uint8_t inputs, uint8_t trio_float, uint8_t out_status);

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
   uint8_t io_inputs = detect_inputs();

    if(io_inputs == 0x3F) { // All IOs as input? maybe there is no chip inserted
        uart_puts("WARNING: All the IOs are floating... Maybe no or broken chip inserted?\n\n");
    }

    wdt_reset();

    setLED(0);

    uint8_t read_1, read_2, trio_floating;


    format_ioconf(io_inputs);

    uart_puts("-------------------------------------------------\n");

    // At worst, if all the IOs are set as inputs, we'll have to try 65536 combinations!
    for(uint32_t idx = 0; idx <= 0xFFFF; idx++) {
        if((idx > 0) && ((idx & ~((uint16_t)io_inputs << 10)) == ((idx - 1) & ~((uint16_t)io_inputs << 10)))) continue; // Skip this round
        
        setLED(1);
        // First, try to force the TRIO to low
        compound_io_write(idx);
        _delay_us(50);
        read_1 = io_read();
        
        // Then try to force them to high
        compound_io_write(0x030000 | idx);
        _delay_us(50);
        read_2 = io_read();

        trio_floating = ((read_1 & 0xC0) ^ (read_2 & 0xC0)) >> 6;

        setLED(0);
        wdt_reset(); // Kick the watchdog

        format_brutef(idx, io_inputs, trio_floating, read_2);
    }

    uart_puts("Execution complete...\n");

    while(1) {
        setLED(1);
        _delay_ms(500);
        wdt_reset(); // Kick the watchdog
        setLED(0);
        _delay_ms(500);
    }


    return 0;
}

static void setLED(uint8_t status) {
    if(!status) LEDPORT &= ~(_BV(LED_P)); // Turn the LED off
    else LEDPORT |= _BV(LED_P); // Turn the LED on
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

static void format_ioconf(uint8_t inputs) {
    memset(str_buf, 0, STR_BUF_SIZE);
    sprintf(str_buf, "The following input config was detected:\n");
    uart_puts(str_buf);
    
    memset(str_buf, 0, STR_BUF_SIZE);
    sprintf(str_buf, "IO1: %c\nIO2: %c\nIO3: %c\nIO4: %c\nIO5: %c\nIO6: %c\n\n", 
    ((inputs >> 0) & 0x01) ? 'I' : 'O',
    ((inputs >> 1) & 0x01) ? 'I' : 'O',
    ((inputs >> 2) & 0x01) ? 'I' : 'O',
    ((inputs >> 3) & 0x01) ? 'I' : 'O',
    ((inputs >> 4) & 0x01) ? 'I' : 'O',
    ((inputs >> 5) & 0x01) ? 'I' : 'O');
    uart_puts(str_buf);
}

static void format_brutef(uint16_t idx, uint8_t inputs, uint8_t trio_float, uint8_t out_status) {
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
        if(!(inputs >> i & 0x01)) *str_ptr = ((out_status >> i) & 0x01) ? '1' : '0';
        else *str_ptr = '.';
        str_ptr++;
        *str_ptr = ' ';
        str_ptr++;
    }

    if(trio_float & 0x01) *str_ptr = 'x';
    else *str_ptr = ((out_status >> 6) & 0x01) ? '1' : '0';
    str_ptr++;
    *str_ptr = ' ';
    str_ptr++;

    if(trio_float & 0x02) *str_ptr = 'x';
    else *str_ptr = ((out_status >> 7) & 0x01) ? '1' : '0';
    str_ptr++;
    *str_ptr = '\n';

    uart_puts(str_buf);
}

ISR(INT1_vect) { // Manage INT1
}