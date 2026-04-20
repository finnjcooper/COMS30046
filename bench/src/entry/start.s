	.section .text
	.globl _start
_start:
	call main
	ecall
1:	j 1b
