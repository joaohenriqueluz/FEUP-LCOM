#ifndef _VBE_MACROS_H_
#define _VBE_MACROS_H_

#define BIT(n) (0x01<<(n))

#define VBE_MODE 0x4f02
#define LINEAR   BIT(14) | 0x105
#define INIT      0x10

#endif
