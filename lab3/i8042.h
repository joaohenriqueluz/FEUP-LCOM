#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/* Keyboard control */

#define BIT(n) (0x01<<(n))

#define KB_IRQ   1

#define KB_STATUS_REG      0x64
#define KBC_CM_REG         0x64
#define OUT_BUF       0x60

#define ESC_BREAK      0x81
#define TWO_BYTE_SCAN     0xe0

#define DELAY_US      20000



#endif /* _LCOM_I8042_H */