#ifndef _SHIFTER_HEADER_
#define _SHIFTER_HEADER_

#include <stdint.h>

// Clears the shifters and enable their outputs
void shifter_init(void);

void shifter_set(uint32_t val);

#endif /* _SHIFTER_HEADER_ */