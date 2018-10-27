#include <machine/asm.h>

.global _C_LABEL(byte)
.global _C_LABEL(erro)
.global _C_LABEL(kbc_asm_ih)

.text
_C_LABEL(kbc_asm_ih):
	
	inb $0x64, %al  //é lido o satus_register
	test $0x01, %al //teste para verificar se o output_buffer está cheio
	jz ERRO

	inb $0x60, %al //é lido o output_buffer
	and $0xc0, %al //corresponde ao comando em c stat &(PAR_ERR | TO_ERR)
	jz FIM
	movb $1, _C_LABEL(erro)
	leave
	ret

FIM:
	movb %al, _C_LABEL(byte) //o valor que foi lido do output_buffer é guardado na variavel global byte
	movb $0, _C_LABEL(erro)
	leave
	ret

ERRO:
	movb $1, _C_LABEL(erro) //a variavel global erro passa a ser 1 pois o programa não correu como esperado
	leave
	ret
