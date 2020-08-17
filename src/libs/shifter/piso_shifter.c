#include "piso_shifter.h"

#include <util/delay.h>
#include <mcu_io.h>

void piso_shifter_init(void) {
    PISO_PORT |= (_BV(PISO_CE) | _BV(PISO_PE)); // Disable the clock (/CE high) and disable the inputs (/PE high)
    PISO_PORT &= ~(_BV(PISO_CLK) | _BV(PISO_CLR)); // CLK and /CLR to low
    _delay_ms(5);
    PISO_PORT |= _BV(PISO_CLR); // Set /CLR high again
}

uint8_t piso_shifter_get(void) {
    return 0;
}