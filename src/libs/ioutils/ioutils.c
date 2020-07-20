#include "ioutils.h"

#include <ioutils/mcu_io.h>
#include <shifter/shifter.h>

void ioutils_write(uint32_t val) {
    shifter_set((uint16_t)(val & 0xFFFF));
    io_write((uint8_t)((val >> 16) & 0x03));
}