; ===== Date Created: 01 December, 2020 ===== 
bits 16
org 0x7c00

CR: equ 0x0d
LF: equ 0x0a

KERNEL_FLAT_ADDRESS: equ 0x00100000

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

		call load_kernel

		; To protected mode and beyond
		enable_protected_mode

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
loadingKernelMessage: db "Info: Loading kernel...", CR, LF, 0
loadedKernelMessage: db "Info: Loaded kernel!", CR, LF, 0

debugMessage: db "Here", CR, LF, 0

; TODO(fkp): Move this somewhere else
; void load_kernel()
load_kernel:
	.setup:
		mov si, loadingKernelMessage
		call print_string

		; Temporary buffer will be 32 sectors at 0x3000
		tempBufferSegment: equ 0x0200
		maxSectorsPerRead: equ 32

		sectorsAlreadyRead: dw 0
		numberOfSectorsToReadNext: dw 0

	.calculate_number_of_sectors:
		mov dx, word [kernelNumberOfSectors]
		sub dx, word [sectorsAlreadyRead]
		cmp dx, 32
		jle .do_read
		mov dx, 32

	.do_read:
		mov [numberOfSectorsToReadNext], dx

		; Sets es:bx to be destination
		mov dx, tempBufferSegment
		mov es, dx
		xor bx, bx
		
		mov cx, word [kernelStartSector]
		add cx, word [sectorsAlreadyRead]
		add cx, 1				; Sectors are one-based
		mov ax, word [numberOfSectorsToReadNext]
		call read_disk

	.read_again_or_finish:
		mov ax, word [sectorsAlreadyRead]
		add ax, word [numberOfSectorsToReadNext]
		mov word [sectorsAlreadyRead], ax

		cmp ax, word [kernelNumberOfSectors]
		jl .calculate_number_of_sectors

	.done:
		mov si, loadedKernelMessage
		call print_string
		ret
