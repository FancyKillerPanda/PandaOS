; ===== Date Created: 01 December, 2020 ===== 
org 0x7c00

CR: equ 0x0d
LF: equ 0x0a

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
	
		mov ax, 0x07e0
		mov es, ax
		xor bx, bx
		mov cl, 2				; Start sector (one-based)
		mov al, [bootloaderNumberOfExtraSectors]
		call read_disk

	.after_expansion:
		jmp $

%include "utility-inl.asm"

; Data
bootDriveNumber: db 0
welcomeMessage: db "PandaOS", CR, LF, 0
rebootMessage: db "Press any key to reboot...", CR, LF, 0
diskErrorMessage: db "Error: Failed to read disk!", CR, LF, 0

end:
	times 504 - ($ - $$) db 0

	; NOTE(fkp): Keep at the end (magic)!
	; TODO(fkp): This should be filled in by the hard disk creator
	bootloaderNumberOfExtraSectors: dw 0
	kernelStartSector: dw 0
	kernelNumberOfSectors: dw 0

	dw 0xaa55
