; ===== Date Created: 27 July, 2021 ===== 
bits 16

CR: equ 0x0d
LF: equ 0x0a
BOOTLOADER_STACK_ADDRESS: equ 0xb000 ; Same as in VBR

MBR_ORIGINAL_ADDRESS: equ 0x7c00
MBR_RELOCATED_ADDRESS: equ 0x7a00

org MBR_RELOCATED_ADDRESS

main:
	.setup:
		; Sets up starting segments
		xor ax, ax
		mov ds, ax
		mov ss, ax
		mov sp, BOOTLOADER_STACK_ADDRESS

	.relocate:
		mov cx, 128				; Repeats 128 times * 4 bytes
		mov si, MBR_ORIGINAL_ADDRESS
		mov di, MBR_RELOCATED_ADDRESS
		rep movsd

		call clear_screen
		mov si, relocatingMBRMessage
		call print_string		

		jmp 0x00:relocated_main

relocated_main:
		xor cx, cx

	.check_partitions:
		cmp byte [si], 0x80
		je .found_active_partition

		inc cx
		cmp cx, 4
		jg .check_partitions

	.no_partition_found:
		mov si, partitionNotFoundMessage
		call print_string
		call reboot

	.found_active_partition:
		mov [secondPartition], si

		mov si, partitionFoundMessage
		call print_string

		mov ax, MBR_ORIGINAL_ADDRESS / 16
		mov es, ax
		xor bx, bx
		mov cx, [selectedPartition]
		add cx, 8				; Offset to LBA
		mov ax, 1
		call read_disk

	.jump_to_vbr:
		mov si, jumpingToVBRMessage
		call print_string

		mov si, [selectedPartition]
		mov dl, [bootDriveNumber]

		jmp 0x7c00

%include "utility-inl.asm"

; Data
bootDriveNumber: db 0
selectedPartition: dw 0
relocatingMBRMessage: db "Info: Relocating MBR!", CR, LF, 0
partitionFoundMessage: db "Info: Found active partition!", CR, LF, 0
partitionNotFoundMessage: db "Error: No active partition!", CR, LF, 0
jumpingToVBRMessage: db "Info: Jumping to VBR!", CR, LF, 0

end_of_mbr_code:
	times 446 - ($ - $$) db 0

; The four partitions
; NOTE(fkp): The data will be filled in by genVDisk
firstPartition:
	.attributes: db 0

	; Start location
	.headBegin: db 0
	.sectorBegin: db 0
	.cylinderBegin: db 0

	.systemID: db 0

	; End location
	.headEnd: db 0
	.sectorEnd db 0
	.cylinderEnd: db 0

	.partitionLBA: dd 0
	.numberOfSectors: dd 0

secondPartition:
	.attributes: db 0

	; Start location
	.headBegin: db 0
	.sectorBegin: db 0
	.cylinderBegin: db 0

	.systemID: db 0

	; End location
	.headEnd: db 0
	.sectorEnd db 0
	.cylinderEnd: db 0

	.partitionLBA: dd 0
	.numberOfSectors: dd 0

thirdPartition:
	.attributes: db 0

	; Start location
	.headBegin: db 0
	.sectorBegin: db 0
	.cylinderBegin: db 0

	.systemID: db 0

	; End location
	.headEnd: db 0
	.sectorEnd db 0
	.cylinderEnd: db 0

	.partitionLBA: dd 0
	.numberOfSectors: dd 0

fourthPartition:
	.attributes: db 0

	; Start location
	.headBegin: db 0
	.sectorBegin: db 0
	.cylinderBegin: db 0

	.systemID: db 0

	; End location
	.headEnd: db 0
	.sectorEnd db 0
	.cylinderEnd: db 0

	.partitionLBA: dd 0
	.numberOfSectors: dd 0

end_of_first_sector:
	dw 0xaa55
