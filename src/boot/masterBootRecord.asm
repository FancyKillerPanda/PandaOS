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

	print relocating_msg
	
	; Far jump to the new address
	jmp 0x0:start

start:
	; We are here after the relocation
	sti
	mov [DriveNumber], dl

	call clear_screen
	
.find_bootable_partition:
	mov bx, first_partition
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
	; TODO(fkp): Check if the BIOS supports LBA addressing

	print found_partition_msg
	
	mov [SelectedPartition], bx
	add bx, PARTITION_LBA_OFFSET
	xor ax, ax
	mov es, ax
	mov eax, [bx]
	mov di, VBR_ADDRESS
	mov dl, [DriveNumber]

	; Reads a single sector
	call extended_read_lba

.jump_to_vbr:
	; Makes the jump
	mov si, [SelectedPartition]
	mov dl, [DriveNumber]

	print jumping_to_vbr_msg
	
	jmp 0x0:VBR_ADDRESS

.no_bootable_partition:
	print no_partition_msg
	call reboot

DriveNumber: db 0
SelectedPartition: db 0

; no_partition_msg: db "No bootable partition.", CR, LF, 0
no_partition_msg: db "No ptn", CR, LF, 0
; relocating_msg: db "Relocating MBR", CR, LF, 0
relocating_msg: db "Rlc", CR, LF, 0
; found_partition_msg: db "Found bootable partition", CR, LF, 0
found_partition_msg: db "Fnd", CR, LF, 0
; jumping_to_vbr_msg: db "Jumping to VBR", CR, LF, 0
jumping_to_vbr_msg: db "Jmp", CR, LF, 0
	
; TODO(fkp): Move this
; eax: start location (LBA)
; es: segment to read into
; di: offset to read into
; dl: boot drive number
extended_read_lba:
	push ax
	push si
	cmp [disk_address_packet.sectors_to_read], word MAX_SECTORS_PER_READ
	jg .read_failed

.initialise_dap:
	mov [disk_address_packet.start_low], eax
	mov [disk_address_packet.segment_to_read_into], es
	mov [disk_address_packet.offset_to_read_into], di
	mov ax, disk_address_packet
	mov si, ax

.read:
	mov ah, 0x42
	int 0x13
	jc .read_failed

	pop si
	pop ax
	ret

.read_failed:
	print disk_error_msg
	call reboot

disk_address_packet:
	.size:					db 16
	.unused:				db 0
	.sectors_to_read:		dw 1
	.offset_to_read_into:	dw 0
	.segment_to_read_into:	dw 0
	.start_low:				dd 0
	.start_high:			dd 0
	
	%include "biosParameterBlock-inl.asm"
	%include "commonUtility-inl.asm"
	
mbr_code_padding: times 446 - ($ - $$) db 0

; The four partitions
first_partition:
	.attributes:		db 0
	.head_begin:		db 0
	.sector_begin:		db 0
	.cylinder_begin:	db 0
	.type:				db 0
	.head_end:			db 0
	.sector_end:		db 0
	.cylinder_end:		db 0
	.lba_address:		dd 0
	.number_of_sectors:	dd 0
	
second_partition:
	.attributes:		db 0
	.head_begin:		db 0
	.sector_begin:		db 0
	.cylinder_begin:	db 0
	.type:				db 0
	.head_end:			db 0
	.sector_end:		db 0
	.cylinder_end:		db 0
	.lba_address:		dd 0
	.number_of_sectors:	dd 0
	
third_partition:
	.attributes:		db 0
	.head_begin:		db 0
	.sector_begin:		db 0
	.cylinder_begin:	db 0
	.type:				db 0
	.head_end:			db 0
	.sector_end:		db 0
	.cylinder_end:		db 0
	.lba_address:		dd 0
	.number_of_sectors:	dd 0
	
fourth_partition:
	.attributes:		db 0
	.head_begin:		db 0
	.sector_begin:		db 0
	.cylinder_begin:	db 0
	.type:				db 0
	.head_end:			db 0
	.sector_end:		db 0
	.cylinder_end:		db 0
	.lba_address:		dd 0
	.number_of_sectors:	dd 0
	
end:
	db 0x55, 0xaa
