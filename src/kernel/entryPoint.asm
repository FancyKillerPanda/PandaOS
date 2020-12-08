; ===== Date Created: 03 December, 2020 ===== 
bits 32

extern kmain

section .entry

; void start()
global start
start:
	mov esp, kernelStackStart
	call kmain

hang:
	cli
	hlt
	jmp hang

section .data
; The kernel stack
; TODO(fkp): Reserve bytes instead of defining them.
; Then this can go in .bss instead of .data
align 16
kernelStackEnd:
	times 16384 db 0
kernelStackStart:
