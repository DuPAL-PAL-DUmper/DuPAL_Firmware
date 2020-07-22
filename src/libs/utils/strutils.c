#include "strutils.h"

uint32_t strutils_str_to_u32(char *str) {
    uint32_t res = 0;

    char c;
    uint8_t diff = 0;
    for(uint8_t idx = 0; idx < 8; idx++) {
        c = str[idx];
        
        if(c >= 0x30 && c <= 0x39) { // A digit
            diff = 0x30;
        } else if(c >= 0x61 && c <= 0x66) { // lowercase letter
            diff = 0x57;
        } else if(c >= 0x41 && c <= 0x46) { // uppercase letter
            diff = 0x37;
        } else return 0; // Bad string
            
        res |= (((uint32_t)(c - diff)) << (28-(4 * idx)));
    }

    return res;
}

void strutils_u8_to_str(char *str, uint8_t data) {
    uint8_t nibble;

    for(uint8_t idx = 0; idx < 2; idx++) {
        nibble = (data >> ((1 - idx)*4)) & 0x0F;
        str[idx] = nibble + (nibble < 0x0A ? 0x30 : 0x37);
    }
}

void strutils_u32_to_str(char *str, uint32_t data) {
    uint8_t nibble;

    for(uint8_t idx = 0; idx < 8; idx++) {
        nibble = (data >> ((7 - idx)*4)) & 0x0F;
        str[idx] = nibble + ((nibble < 0x0A) ? 0x30 : 0x37);
    }
}