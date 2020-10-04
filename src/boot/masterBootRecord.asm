; ===== Date Created: 20 September, 2020 ===== 
bits 16

MBR_ORIGINAL_ADDRESS: equ 0x7c00
MBR_RELOCATED_ADDRESS: equ 0x0500
VBR_ADDRESS: equ 0x7c00
PARTITION_ENTRY_SIZE: equ 0x10
PARTITION_LBA_OFFSET: equ 0x08
	
org MBR_RELOCATED_ADDRESS
	
%include "macros-inl.asm"
	
relocate:
	; Clears the initial registers
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, MBR_ORIGINAL_ADDRESS

	; Relocates the MBR
	push ds
	mov ds, ax
	mov cx, 0x200				; Repeating 512 times
	mov si, MBR_ORIGINAL_ADDRESS
	mov di, MBR_RELOCATED_ADDRESS
	rep movsb
	pop ds

	; Logging
	pusha
	call clear_screen
	popa
	log loadingMessage
	log relocatingMessage
	
	; Far jump to the new address
	jmp 0x0:start

start:
	; We are here after the relocation
	sti
	mov [driveNumber], dl

.find_bootable_partition:
	mov bx, firstPartition
	mov cx, 4					; We want to look through four partitions

.check_partition:
	cmp byte [bx], 0x80			; Checks the active bit
	je .found_active_partition

	; Tries again
	add bx, PARTITION_ENTRY_SIZE
	dec cx
	jnz .check_partition
	jmp .no_bootable_partition	; When cx reaches zero

.found_active_partition:
	log foundPartitionMessage
	mov [selectedPartition], bx
	
	; Checks if the BIOS supports LBA addressing
	mov ah, 0x41
	mov bx, 0x55aa
	int 0x13
	jc .lba_not_supported
	
	mov bx, [selectedPartition]
	add bx, PARTITION_LBA_OFFSET
	xor ax, ax
	mov es, ax
	mov eax, [bx]
	mov di, VBR_ADDRESS
	mov dl, [driveNumber]

	; Reads a single sector
	call extended_read_lba

.jump_to_vbr:
	; Makes the jump
	mov si, [selectedPartition]
	mov dl, [driveNumber]

	log jumpingToVBRMessage
	
	jmp 0x0:VBR_ADDRESS

.no_bootable_partition:
	print noPartitionMessage
	call reboot

.lba_not_supported:
	print diskErrorMessage
	call reboot

driveNumber: db 0
selectedPartition: db 0

loadingMessage: db "PandaOS", CR, LF, 0
; noPartitionMessage: db "No bootable partition", CR, LF, 0
noPartitionMessage: db "No partition", CR, LF, 0
relocatingMessage: db "Relocating MBR", CR, LF, 0
; foundPartitionMessage: db "Found bootable partition", CR, LF, 0
foundPartitionMessage: db "Found", CR, LF, 0
; jumpingToVBRMessage: db "Jumping to VBR", CR, LF, 0
jumpingToVBRMessage: db "Jump", CR, LF, 0

%define UTILITY_NO_READ_SECTOR
%define UTILITY_NO_BOOT_FAILED
%define UTILITY_NO_RESET_DISK_SYSTEM
%include "commonUtility-inl.asm"
%include "biosParameterBlock-inl.asm"
	
MBRCodePadding: times 446 - ($ - $$) db 0

; The four partitions
firstPartition:
	.attributes:		db 0
	.headBegin:		db 0
	.sectorBegin:		db 0
	.cylinderBegin:	db 0
	.type:				db 0
	.headEnd:			db 0
	.sectorEnd:		db 0
	.cylinderEnd:		db 0
	.LBAAddress:		dd 0
	.numberOfSectors:	dd 0
	
secondPartition:
	.attributes:		db 0
	.headBegin:		db 0
	.sectorBegin:		db 0
	.cylinderBegin:	db 0
	.type:				db 0
	.headEnd:			db 0
	.sectorEnd:		db 0
	.cylinderEnd:		db 0
	.LBAAddress:		dd 0
	.numberOfSectors:	dd 0
	
thirdPartition:
	.attributes:		db 0
	.headBegin:		db 0
	.sectorBegin:		db 0
	.cylinderBegin:	db 0
	.type:				db 0
	.headEnd:			db 0
	.sectorEnd:		db 0
	.cylinderEnd:		db 0
	.LBAAddress:		dd 0
	.numberOfSectors:	dd 0
	
fourthPartition:
	.attributes:		db 0
	.headBegin:		db 0
	.sectorBegin:		db 0
	.cylinderBegin:	db 0
	.type:				db 0
	.headEnd:			db 0
	.sectorEnd:		db 0
	.cylinderEnd:		db 0
	.LBAAddress:		dd 0
	.numberOfSectors:	dd 0
	
end:
	db 0x55, 0xaa
