#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/* Keyboard and Mouse control */

#define BIT(n) (0x01<<(n))

#ifdef LAB3
int sys_inb_cnt(port_t port, uint32_t *byte);
#else
#define sys_inb_cnt(p,q) sys_inb(p,q)
#endif

#define KB_IRQ   1
#define MOUSE_IRQ  12

#define KB_STATUS_REG      0x64
#define KBC_CM_REG         0x64
#define KBC_READ           0x20
#define WRITE_BYTE         0x60
#define KB_ENABLE 		   BIT(0)

#define OUT_BUF      	   0x60

#define ESC_BREAK          0x81
#define TWO_BYTE_SCAN      0xe0

#define DELAY_US      	   20000
#define TEMP_HOOK_MOUSE    3

#define	OBF 		       BIT(0)
#define IBF 		       BIT(1)
#define AUX                BIT(5)
#define PAR_ERR 	       BIT(7)
#define TO_ERR 		       BIT(6)

#define WRITE_TO_MOUSE     0xd4
#define READ_DATA 		   0xeb
#define DISABLE_MOUSE      0xf5
#define STREAM_MODE        0xea
#define MOUSE_DEFAULT      0xf6
#define REMOTE_MODE        0xf0
#define ENABLE_DR          0xF4
#define MOUSE_RESET        0xff
//#define DISABLE_MOUSE      0xa7
#define DISABLE_DATA_REPORTING   0xf5
#define ACK     0xfa
#define NACK    0xfe
#define ERROR   0xfc



#endif /* _LCOM_I8042_H */
