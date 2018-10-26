#include <machine/asm.h>
#include "i8042.h"

.global _C_LABEL(byte)
.global _C_LABEL(erro)
.global _C_LABEL(kbc_asm_ih)

.data
_C_LABEL(byte):
	.byte 0

_C_LABEL(erro):
	.long 1

.text
_C_LABEL(kbc_asm_ih):
	
	inb $KB_STATUS_REG, %al
	andl %al, $OBF
	jz .erro

	inb $OUT_BUF, %edx
	movb $(PAR_ERR | TO_ERR), %ecx
	andl %ecx, %edx
	jz .fim



.fim:
	movb %edx, _C_LABEL(byte)
	leave
	ret

.erro:
	movb $0, _C_LABEL(erro)
	leave
	ret
