#include "main.h"

#include <stdint.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <pal_types/PAL16L8.h>
#include <ioutils/mcu_io.h>
#include <ioutils/ioutils.h>

#include <uart/uart.h> 
#include <shifter/shifter.h>


#define VERSION "0.0.3"
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
    
    // Prepare the shifter
    shifter_init();
    
    // Enable interrupts
    sei();

    uart_puts(SOFT_HEADER); // Print the header

    pal16l8_analyze();

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

ISR(INT1_vect) { // Manage INT1
}