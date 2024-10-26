[BITS 32]
section .bss
align 4096
stack_bottom:
resb 16384   ;da stack
stack_top:

section .text
global _start:
_start:
	mov esp, stack_top ;here you go cpu, point to this
	extern main        ;c code
	call main          ;call my c code
hang:                      ;catch the cpu if it passes my main call
	hlt
	cli
	jmp hang
