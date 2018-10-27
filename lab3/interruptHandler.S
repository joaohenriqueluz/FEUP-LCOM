#include <machine/asm.h>

.global _C_LABEL(byte)
.global _C_LABEL(erro)
.global _C_LABEL(kbc_asm_ih)

.text
_C_LABEL(kbc_asm_ih):
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx

INICIO:
	inb $0x64, %al  
	testb $0x01, %al
	jz INICIO

	testb $0xc0, %al
	jz FIM
	movb $1, _C_LABEL(erro)
	leave
	ret

FIM:
	inb $0x60, %al
	movb %al, _C_LABEL(byte)
	movb $0, _C_LABEL(erro)
	leave
	ret

ERRO:
	movb $1, _C_LABEL(erro)
	leave
	ret
