#include "compound_io.h"

#include <shifter.h>
#include <ioconfig.h>

void compound_io_write(uint32_t val) {
    shifter_set((uint16_t)(val & 0xFFFF));
    trio_write((uint8_t)((val >> 16) & 0x03));
}