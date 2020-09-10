; ===== Date Created: 07 September, 2020 ===== 
bits 16

%include "macros-inl.asm"
	
main:
	print kernel_loader_msg

	; Global descriptor table set up at 0x0000:0x0800
	xor ax, ax
	mov es, ax
	mov di, 0x80

	; Tells the CPU where the GDT table is and its size
	lgdt [gdt_entry]
	
hang:
	jmp hang

	
boot_failure:
	print disk_error_msg
	call reboot

	
gdt_start:
	; The NULL descriptor
	dq 0x0000000000000000

	; The code segment descriptor
	dw 0xffff					; Limit of 4GB
	dw 0x0000					; Base of zero
	db 0x00						; More base bits
	db 0x9a						; Access (present, executable, and readable are set)
	db 0xcf						; Flags (granularity and size) and limit
	db 0x00						; More base bits

	; The data segment descriptor
	dw 0xffff					; Limit of 4GB
	dw 0x0000					; Base of zero
	db 0x00						; More base bits
	db 0x92						; Access (present and writable are set)
	db 0xcf						; Flags (granularity and size) and limit
	db 0x00						; More base bits
gdt_end:

gdt_entry:
	dw gdt_end - gdt_start - 1
	dd gdt_start


idt_entry:
	dw 0x0800					; 2048 bytes (for 256 interrupts)
	dd 0x00000000				; Starts at location zero

	
%include "biosParameterBlock-inl.asm"
%include "utility-inl.asm"

kernel_loader_msg: db "PandaOS kernel loader...", CR, LF, 0
	
end:
	; Some padding to make sure we can load more than one sector
	times 1000 db 0
