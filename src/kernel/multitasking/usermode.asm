; ===== Date Created: 26 July, 2021 ===== 
bits 32

GDT_RING_3_CODE_SELECTOR: equ 40
GDT_RING_3_DATA_SELECTOR: equ 48

; no-return jump_to_usermode(whereTo)
global jump_to_usermode
jump_to_usermode:
	.setup:
		pop ebx					; Return address (unneeded)
		pop ebx					; First parameter

	.segments:
		; ss will be handled by iret
		mov ax, GDT_RING_3_DATA_SELECTOR | 3
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax

	.stack_frame:
		; Sets up the stack frame that iret expects
		mov eax, esp
		push GDT_RING_3_DATA_SELECTOR | 3
		push eax
		pushfd
		push GDT_RING_3_CODE_SELECTOR | 3
		push ebx

		xchg bx, bx

		iretd
