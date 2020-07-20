#ifndef _STRUTILS_HEADER_
#define _STRUTILS_HEADER_

#include <stdint.h>

void strutils_print_6io_conf(uint8_t inputs);
void strutils_print_pinstat(uint16_t idx, uint8_t inputs, uint8_t floating, uint8_t out_status, char special_symbol);

#endif /* _STRUTILS_HEADER_ */