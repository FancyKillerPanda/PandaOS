; ===== Date Created: 03 December, 2020 ===== 
bits 32

extern kmain

section .entry

; void start(eax: memory map)
global start
start:
	mov esp, kernelStackStart

	push eax					; Video mode info
	push ebx					; Memory map
	push ecx					; Number of lines printed

	call kmain

hang:
	cli
	hlt
	jmp hang

section .bss

; The kernel stack
align 16
kernelStackEnd:
	resb 16384
kernelStackStart:

section .magic

global pandaOSMagicString
pandaOSMagicString: db "PandaOS Magic!"
