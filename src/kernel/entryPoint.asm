; ===== Date Created: 03 December, 2020 ===== 
bits 32

extern kmain

; void start()
global start
start:
	mov esp, kernelStackStart
	call kmain

hang:
	cli
	hlt
	jmp hang

; The kernel stack
align 16
kernelStackEnd:
	times 16384 db 0
kernelStackStart:
