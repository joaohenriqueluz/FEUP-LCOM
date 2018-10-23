#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/* Keyboard control */

#define BIT(n) (0x01<<(n))

#define KB_IRQ   1

#define KB_STATUS_REG      0x64
#define KBC_CM_REG         0x64
#define OUT_BUF       0x60

#define ESC_BREAK         0x81
#define TWO_BYTE_SCAN     0xe0

#define DELAY_US      20000
#define TEMP_HOOK     1

#define TEMP_HOOK 1

#define	OBF 		BIT(0)
#define AUX         BIT(5)
#define PAR_ERR 	BIT(7)
#define TO_ERR 		BIT(6)



#endif /* _LCOM_I8042_H */
