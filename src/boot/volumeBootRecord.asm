; ===== Date Created: 01 December, 2020 ===== 
bits 16
org 0x7c00

CR: equ 0x0d
LF: equ 0x0a

%macro enable_protected_mode 0
	%%.setup:
		mov si, enableProtectedModeMessage
		call print_string

		; NOTE(fkp): This requires that we have already
		; described the GDT and IDT tables.
		call load_descriptor_tables

	%%.enable:
		; Enables protected mode
		mov eax, cr0
		or eax, 1
		mov cr0, eax

		; Also clears the prefetch queue
		jmp gdtCode32Offset:%%.setup_segments
		nop
		nop

	%%.setup_segments:
		bits 32
		; Selects the data descriptor for all segments (except cs)
		mov ax, gdtData32Offset
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax
		mov esp, 0x20000		; Still within usable memory
%endmacro

%macro enable_real_mode 0
	%%.setup:
		cli

		; Also clears the prefetch queue
		jmp gdtCode16Offset:%%.switch_to_16_bit_gdt
		nop
		nop

	%%.switch_to_16_bit_gdt:
		bits 16
		mov eax, gdtData16Offset
		mov ds, eax
		mov es, eax
		mov fs, eax
		mov gs, eax
		mov ss, eax

	%%.disable_protected_mode:
		mov eax, cr0
		and eax, 0x7ffffffe		; Disables protected mode and paging
		mov cr0, eax

		; Also clears the prefetch queue
		jmp 0x00:%%.reset_gdt_selector
		nop
		nop

	%%.reset_gdt_selector:
		xor ax, ax
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax
		mov sp, 0x7c00

	%%.load_real_mode_idt:
		lidt [idtEntryRealMode]

	%%.cleanup:
		xchg bx, bx
		sti
		mov si, enableRealModeMessage
		call print_string
%endmacro

start:
	jmp short main
	nop

; There is a short form of the FAT32 BPB
; biosParameterBlock: times 68 db 0
biosParameterBlock: times 87 db 0
	
main:
	.setup:
		mov [bootDriveNumber], dl

		mov si, welcomeMessage
		call print_string

		; We don't want to try expand if the bootloader
		; fits in one sector
		cmp byte [bootloaderNumberOfExtraSectors], 0
		je .after_expansion

		mov si, expandingMessage
		call print_string

		mov ax, 0x07e0
		mov es, ax
		xor bx, bx
		mov cl, 2				; Start sector (one-based)
		mov al, [bootloaderNumberOfExtraSectors]
		call read_disk

	.after_expansion:
		; A20 line
		call try_enable_a20

		; Desriptor tables
		call describe_gdt
		call describe_idt

		; To protected mode and beyond
		enable_protected_mode

		; Debug testing
		call debug_test_mode_switching

		jmp $

bits 16
%include "utility-inl.asm"

; Data (to be used by bootloader expander)
bootDriveNumber: db 0
welcomeMessage: db "PandaOS", CR, LF, 0
expandingMessage: db "Info: Expanding bootloader...", CR, LF, 0
rebootMessage: db "Press any key to reboot...", CR, LF, 0
diskErrorMessage: db "Error: Failed to read disk!", CR, LF, 0

end_of_first_sector:
	times 504 - ($ - $$) db 0

	; NOTE(fkp): Keep at the end (magic)!
	bootloaderNumberOfExtraSectors: dw 0
	kernelStartSector: dw 0
	kernelNumberOfSectors: dw 0

	dw 0xaa55

%include "a20Utility-inl.asm"
%include "descriptorTableUtility-inl.asm"

; Data (to be used by the extended bootloader)
a20SuccessMessage: db "Info: Enabled A20 line!", CR, LF, 0
a20FailedMessage: db "Error: Failed to enable A20 line!", CR, LF, 0
enableProtectedModeMessage: db "Info: Enabling protected mode!", CR, LF, 0
enableRealModeMessage: db "Info: Enabled real mode!", CR, LF, 0

bits 32
debug_test_mode_switching:
	enable_real_mode
	enable_protected_mode
	enable_real_mode
	enable_protected_mode
	enable_real_mode
	enable_protected_mode
	ret
