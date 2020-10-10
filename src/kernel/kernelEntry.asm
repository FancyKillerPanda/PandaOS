; ===== Date Created: 10 September, 2020 ===== 
bits 32

extern start_kernel
	
global start
start:
	mov esp, kernel_stack_start
	push eax					; The memory map pointer
	call start_kernel

hang:
	cli
	hlt
	jmp hang

align 16
kernel_stack_end:
	times 16384 db 0
kernel_stack_start:
