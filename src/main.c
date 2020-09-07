#include "main.h"

#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <pal_types/remote_control.h>

#include <ioutils/mcu_io.h>
#include <ioutils/ioutils.h>

#include <uart/uart.h> 
#include <shifter/sipo_shifter.h>
#include <shifter/piso_shifter.h>


#define VERSION "0.1.1"
#define SOFT_HEADER "\nDuPAL - " VERSION "\n\n"

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
    
    // Prepare the shifters
    sipo_shifter_init();
    piso_shifter_init();
    
    // Enable interrupts
    sei();


    uart_puts(SOFT_HEADER); // Print the header

    void (*pal_analyzer)(void) = NULL;

    ioutils_setLED(ACT_LED, 0);

    pal_analyzer = remote_control_analyze;
    (*pal_analyzer)();

    uart_puts("DONE.\n");

    // We're done, blink the led at 1Hz and wait for a watchdog reset
    while(1) {
        _delay_ms(1000);
        ioutils_setLED(ACT_LED, 0);
        _delay_ms(1000);
    }


    return 0;
}

ISR(INT1_vect) { // Manage INT1
}