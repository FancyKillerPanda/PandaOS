; ===== Date Created: 01 December, 2020 ===== 
bits 16
org 0x7c00

CR: equ 0x0d
LF: equ 0x0a
BOOTLOADER_STACK_ADDRESS: equ 0xb000

%include "kernelLoadMacros-inl.asm"

start:
	jmp short main
	nop

; There is a short form of the FAT32 BPB
; biosParameterBlock: times 68 db 0
biosParameterBlock: times 87 db 0
	
main:
	.setup:
		; Sets up starting segments
		xor ax, ax
		mov ds, ax
		mov es, ax
		mov ss, ax
		mov sp, BOOTLOADER_STACK_ADDRESS

		mov [bootDriveNumber], dl

		call clear_screen
		mov si, welcomeMessage
		call print_string

	.expand_bootloader:
		; We don't want to try expand if the bootloader
		; fits in one sector
		cmp byte [bootloaderNumberOfExtraSectors], 0
		je .after_expansion

		mov si, expandingMessage
		call print_string

		mov ax, 0x07e0
		mov es, ax
		xor bx, bx
		mov cl, 1				; Start sector
		mov al, [bootloaderNumberOfExtraSectors]
		call read_disk

	.after_expansion:
		jmp expanded_main

%include "utility-inl.asm"

; Data (to be used by bootloader expander)
bits 16
bootDriveNumber: db 0
welcomeMessage: db "PandaOS", CR, LF, 0
expandingMessage: db "Info: Expanding bootloader...", CR, LF, 0
rebootMessage: db "Press any key to reboot...", CR, LF, 0
diskErrorMessage: db "Error: Failed to read disk!", CR, LF, 0

numberOfLinesPrinted: db 0

end_of_first_sector:
	times 504 - ($ - $$) db 0

	; NOTE(fkp): Keep at the end (magic)!
	bootloaderNumberOfExtraSectors: dw 0
	kernelStartSector: dw 0
	kernelNumberOfSectors: dw 0

	dw 0xaa55

expanded_main:
	.prepare_kernel:
		; A20 line
		call try_enable_a20

		; Memory map
		call get_memory_map

		; Video mode info
		call get_edid_info
		call get_vesa_bios_info
		call select_vesa_mode
		call set_vesa_mode

		mov word [videoMode.screenWidth], 960
		mov word [videoMode.screenHeight], 540
		mov word [videoMode.bitsPerPixel], 24

		; Desriptor tables
		call describe_gdt
		call describe_idt

		; To the kernel and beyond!
		call load_kernel
		enable_protected_mode

	.set_up_paging:
		call init_paging_structures
		call identity_map_kernel
		call start_paging

	.jump:
		; Passes parameters to the kernel
		mov eax, videoMode
		mov ebx, memoryMap
		movzx ecx, byte [numberOfLinesPrinted]
		jmp HIGHER_HALF_OFFSET + KERNEL_FLAT_ADDRESS

		; Should never get here
		jmp $

%include "a20Utility-inl.asm"
%include "descriptorTableUtility-inl.asm"
%include "kernelLoadUtility-inl.asm"
%include "memoryMapUtility-inl.asm"
%include "pagingUtility-inl.asm"
%include "videoModeUtility-inl.asm"

; Data (to be used by the extended bootloader)
bits 16
a20SuccessMessage: db "Info: Enabled A20 line!", CR, LF, 0
a20FailedMessage: db "Error: Failed to enable A20 line!", CR, LF, 0
enableProtectedModeMessage: db "Info: Enabling protected mode!", CR, LF, 0
enableRealModeMessage: db "Info: Enabled real mode!", CR, LF, 0
loadingKernelMessage: db "Info: Loading kernel...", CR, LF, 0
loadedKernelMessage: db "Info: Loaded kernel!", CR, LF, 0
memoryMapNotDetectedMessage: db "Error: Memory map not detected!", CR, LF, 0
memoryMapFinishedMessage: db "Info: Finished reading memory map.", CR, LF, 0

bootloaderStackPointer: dw 0

memoryMapLocation:
