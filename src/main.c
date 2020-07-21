#include "main.h"

#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <pal_types/PAL16L8.h>
#include <pal_types/PAL12L6.h>

#include <ioutils/mcu_io.h>
#include <ioutils/ioutils.h>

#include <uart/uart.h> 
#include <shifter/shifter.h>


#define VERSION "0.0.6"
#define SOFT_HEADER "\nDuPAL - " VERSION "\n\n"

static void print_supported_pal(void);

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
    
    // Prepare the shifter
    shifter_init();
    
    // Enable interrupts
    sei();


    uart_puts(SOFT_HEADER); // Print the header

    print_supported_pal();

    void (*pal_analyzer)(void) = NULL;

    ioutils_setLED(0);

    while(1) {
        if(uart_charavail()) {
            char sel = uart_getchar();
        
            switch(sel) {
                case 'a':
                    pal_analyzer = pal16l8_analyze;
                    break;
                case 'b':
                    pal_analyzer = pal16l8_analyze_dyn;
                    break;
                case 'c':
                    pal_analyzer = pal12l6_analyze;
                    break;
                default:
                    uart_puts("Current selection not supported.\n");
                    pal_analyzer = NULL;
                    break;
            }

            if(pal_analyzer) break; // Get out of here!
        }
        wdt_reset();
    }

    (*pal_analyzer)();

    uart_puts("Analisys complete.\n");

    // We're done, blink the led at 1Hz
    while(1) {
        _delay_ms(1000);
        wdt_reset(); // Kick the watchdog
        ioutils_setLED(0);
        _delay_ms(1000);
        wdt_reset(); // Kick the watchdog
    }


    return 0;
}

static void print_supported_pal(void) {
    uart_puts("Select which PAL type to analyze:\n");
    uart_puts("---------------------------------\n");
    uart_puts("a) PAL16L8/PAL10L8\n");
    uart_puts("b) PAL16L8 - test hi-z outputs as inputs\n");
    uart_puts("c) PAL12L6\n");
    uart_puts("Press the corresponding letter to start analisys.\n\n");
}

ISR(INT1_vect) { // Manage INT1
}