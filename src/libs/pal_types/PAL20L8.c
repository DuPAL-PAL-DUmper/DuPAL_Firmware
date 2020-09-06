#include "PAL20L8.h"

#include <string.h>
#include <stdio.h>

#include <avr/wdt.h>
#include <util/delay.h>

#include <uart/uart.h>
#include <ioutils/ioutils.h>
#include <utils/strutils.h>

static uint8_t detect_inputs(void);
static void print_6io_conf(uint8_t inputs);
static void print_pinstat(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status);

static uint8_t calculate_totInputs(uint8_t io_mask);
static uint8_t calculate_totOutputs(uint8_t io_mask);
static void print_ioINLabels(uint8_t io_mask);
static void print_ioOUTLabels(uint8_t io_mask);
static void print_ioPhase(uint8_t totOuts);

void pal20l8_analyze(void) {
    uart_puts("-[ PAL20L8 analyzer ]-\n");

    ioutils_setLED(ACT_LED, 1); // Turn Activity LED on 
    ioutils_setLED(P24_LED, 1); // Turn P24 LED on

    uart_puts("Detecting inputs... This may take a long time.\n");

    // Detect the inputs on the PAL
    uint8_t io_inputs = detect_inputs();

    if(io_inputs == 0x7E) { // All IOs as input? maybe there is no chip inserted
        uart_puts("WARNING: All the IOs are floating... Maybe no or broken chip inserted?\n\n");
    }

    wdt_reset();

    print_6io_conf(io_inputs);

    // Reset the watchdog and blink a bit
    for(uint8_t i = 0; i < 5; i++) {
        ioutils_setLED(ACT_LED, 1);
        _delay_ms(500);
        ioutils_setLED(ACT_LED, 0);
        _delay_ms(500);
        wdt_reset();
    }

    uart_puts(MARKER_STRING);
    sprintf(str_buf, ".i %u\n", calculate_totInputs(io_inputs)); uart_puts(str_buf);
    sprintf(str_buf, ".o %u\n", calculate_totOutputs(io_inputs)*2); uart_puts(str_buf); // We have both the outputs and the "output enable" pin
    print_ioINLabels(io_inputs);
    print_ioOUTLabels(io_inputs);
    print_ioPhase(calculate_totOutputs(io_inputs));
    uart_puts("\n");

    uint8_t read_1, read_2, floating;
    // At worst, if all the IOs are set as inputs, we'll have to try 65536 combinations!
    for(uint32_t idx = 0; idx <= 0xFFFF; idx++) {
        wdt_reset(); // Kick the watchdog

        if((idx >> 10) & (~io_inputs & 0x3F)) continue; // Skip this round

        ioutils_setLED(ACT_LED, 1);
        // First, try to force the the IOs and outputs to low
        ioutils_write(idx);
        _delay_us(50);
        read_1 = ioutils_read();

        // Then try to force them to high
        // We will also try to force the pins that we did not detect as input in the beginning
        ioutils_write(0x030000 | (((uint32_t)(~io_inputs & 0x3F)) << 10) | idx);
        _delay_us(50);
        read_2 = ioutils_read();

        floating = (read_1 ^ read_2);

        ioutils_setLED(ACT_LED, 0);

        print_pinstat(idx, io_inputs, floating, read_2);
    }

    uart_puts(".e\n");
    uart_puts(MARKER_STRING);
    
    ioutils_setLED(P24_LED, 0); // Turn P24 LED off
}

static void print_ioOUTLabels(uint8_t io_mask) {
    io_mask = ~io_mask & 0x3F;
    uart_puts(".ob ");
    for(uint8_t idx = 0; idx < 6; idx++) {
        if((io_mask >> idx) & 0x01) {
            sprintf(str_buf, "io%u ", 18 - idx);
            uart_puts(str_buf);
        }
    }

    uart_puts("o19 o12 ");
    for(uint8_t idx = 0; idx < 6; idx++) {
        if((io_mask >> idx) & 0x01) {
            sprintf(str_buf, "io%uoe ", 18 - idx);
            uart_puts(str_buf);
        }
    }

    uart_puts("o19oe o12oe \n");
}

static void print_ioPhase(uint8_t totOuts) {
    uart_puts(".phase ");
    for(uint8_t idx = 0; idx < totOuts; idx++) uart_puts("0"); // The outputs
    for(uint8_t idx = 0; idx < totOuts; idx++) uart_puts("1"); // And the OEs
    uart_puts("\n");
}

static void print_ioINLabels(uint8_t io_mask) {
    uart_puts(".ilb i1 i2 i3 i4 i5 i6 i7 i8 i9 i11 ");
    for(uint8_t idx = 0; idx < 6; idx++) {
        if((io_mask >> idx) & 0x01) {
            sprintf(str_buf, "io%u ", 18 - idx);
            uart_puts(str_buf);
        }
    }

    uart_puts("\n");
}

static uint8_t calculate_totInputs(uint8_t io_mask) {
    uint8_t additional_inputs = 0;

    for(uint8_t idx = 0; idx < 6; idx++) additional_inputs += ((io_mask >> idx) & 0x01);

    return 10 + additional_inputs;
}

static uint8_t calculate_totOutputs(uint8_t io_mask) {
    uint8_t additional_outputs = 0;

    io_mask = ~io_mask & 0x3F;
    for(uint8_t idx = 0; idx < 6; idx++) additional_outputs += ((io_mask >> idx) & 0x01);

    return 2 + additional_outputs;
}

static uint8_t detect_inputs(void) {
    uint8_t read1, read2;
    uint8_t inputs = 0xFF;

    ioutils_setLED(ACT_LED, 1); // Activity LED

    for(uint32_t idx = 0; idx < 0x3E07FF; idx++) {
        if(idx & 0x1F800) continue; // Do not set the I/O pins

        ioutils_write(idx); // Zero the potential outputs
        _delay_us(50);
        read1 = ioutils_read();
        ioutils_write(0x1F800 | idx); // Pull high all the IOx pins on the PAL, the rest of the address will remain the same
        _delay_us(50);
        read2 = ioutils_read();

        inputs &= ((read1 ^ read2) & 0x7E);

        wdt_reset();

        if(!(inputs & 0x7E)) break; // We already found that all the I/Os are outputs

        // Print something so we show we're alive...
        if(!(idx % 100)) {
            uart_putchar('\b');
            uart_putchar(get_whirlChar(idx / 100));
        }
    }

    // Delete the whirl
    uart_puts("\b \n");

    ioutils_setLED(ACT_LED, 0);

    return inputs;
}

static void print_6io_conf(uint8_t inputs) {
    memset(str_buf, 0, STR_BUF_SIZE);
    sprintf(str_buf, "Detected IO config:\n");
    uart_puts(str_buf);
    
    uart_puts("IO21 IO20 IO19 IO18 IO17 IO16\n");
    memset(str_buf, 0, STR_BUF_SIZE);
    sprintf(str_buf, " %c    %c    %c    %c    %c    %c\n\n", 
    ((inputs >> 6) & 0x01) ? 'I' : 'O',
    ((inputs >> 5) & 0x01) ? 'I' : 'O',
    ((inputs >> 4) & 0x01) ? 'I' : 'O',
    ((inputs >> 3) & 0x01) ? 'I' : 'O',
    ((inputs >> 2) & 0x01) ? 'I' : 'O',
    ((inputs >> 1) & 0x01) ? 'I' : 'O');
    uart_puts(str_buf);
}

static void print_pinstat(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status) {
    memset(str_buf, 0, STR_BUF_SIZE);

    char *str_ptr = str_buf;
    // Print the inputs
    for(uint8_t i = 0; i < 10; i++) {
        *str_ptr = ((idx >> i) & 0x01) ? '1' : '0';
        str_ptr++;
    }

    // Print IOs as inputs
    for(uint8_t i = 0; i < 6; i++) {
        if(((inputs >> i) & 0x01)) {
            *str_ptr = ((idx >> (10+i)) & 0x01) ? '1' : '0';
            str_ptr++;
        }
    }

    *str_ptr = ' ';
    str_ptr++;

    // Print IO as outputs
    for(uint8_t i = 0; i < 6; i++) {
        if((floating >> i) & 0x01) { *str_ptr = '-'; str_ptr++; } // Check that this output is not in high impedence mode
        else if(!(inputs >> i & 0x01)) { *str_ptr = ((out_status >> i) & 0x01) ? '1' : '0';  str_ptr++; };
    }

    // Pin 19 - output
    if(floating & 0x40) *str_ptr = '-';
    else *str_ptr = ((out_status >> 6) & 0x01) ? '1' : '0';
    str_ptr++;

    // Pin 12 - output
    if(floating & 0x80) *str_ptr = '-';
    else *str_ptr = ((out_status >> 7) & 0x01) ? '1' : '0';
    str_ptr++;
    
    // Print IO high impedence
    for(uint8_t i = 0; i < 6; i++) {
        if((floating >> i) & 0x01) { *str_ptr = '0'; str_ptr++; } // Check that this output is not in high impedence mode
        else if(!(inputs >> i & 0x01)) { *str_ptr = '1';  str_ptr++; };
    }
    
    // Pin 19 - high impedence
    if(floating & 0x40) *str_ptr = '0';
    else *str_ptr = '1';
    str_ptr++;
    
    // Pin 12 - high impedence
    if(floating & 0x80) *str_ptr = '0';
    else *str_ptr = '1';
    str_ptr++;

    *str_ptr = '\n';

    uart_puts(str_buf);
}