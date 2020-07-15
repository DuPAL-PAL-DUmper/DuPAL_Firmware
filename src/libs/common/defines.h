#ifndef _AVR_EXPERIMENTS_DEFINES_
#define _AVR_EXPERIMENTS_DEFINES_

// token pasting
#define token_paste2_int(x, y) x ## y
#define token_paste2(x, y) token_paste2_int(x, y)
#define token_paste3_int(x, y, z) x ## y ## z
#define token_paste3(x, y, z) token_paste3_int(x, y, z)

#endif /* _AVR_EXPERIMENTS_DEFINES_ */
