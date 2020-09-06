#ifndef _SIPO_SHIFTER_HEADER_
#define _SIPO_SHIFTER_HEADER_

#include <stdint.h>

// Clears the shifters and enable their outputs
void sipo_shifter_init(void);

void sipo_shifter_set(uint32_t val);

#endif /* _SIPO_SHIFTER_HEADER_ */