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
	lidt [idt_entry]

	; Tries to enable to A20 line
	try_enable_a20
	
hang:
	jmp hang
	
	
%include "biosParameterBlock-inl.asm"
%include "commonUtility-inl.asm"
%include "loaderUtility-inl.asm"

kernel_loader_msg: db "Info: PandaOS kernel loader...", CR, LF, 0
a20_failed_msg: db "Error: Failed to enable A20 line!", CR, LF, 0
a20_success_msg: db "Info: Enabled A20 line!", CR, LF, 0
