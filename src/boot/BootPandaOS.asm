; ===== Date Created: 05 September, 2020 ===== 
bits 16							; We are working in 16-bit real mode
org 0x7c00						; Where all absolute addresses start

main:
	; Two bytes (jmp) plus one byte (nop)
	; jmp short start				; Out of range...
	jmp start
	nop

%include "biosParameterBlock-inl.asm"
%include "macros-inl.asm"
	
start:
	cli							; Disables interrupts
	mov [BootDriveNumber], dl	; Save the drive we booted from
	
	; Clears registers
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov sp, 0x7c00				; Stack grows from 0x7c00 toward 0x0000
	sti							; Re-enables interrupts

	call clear_screen
;	mov si, loading_msg
;	call print_string
	
	; Resets the disk system
	mov dl, [BootDriveNumber]
	call reset_disk_system
	jc boot_failed

	; Finds the second-stage bootloader file
	mov si, bootloader_file
	find_file: find_file_on_disk
	mov [bootloader_cluster], ax

	fat: load_fat

	; Reads the second-stage bootloader file
	mov cx, [bootloader_cluster]
	mov ax, BOOTLOADER_SEGMENT
	mov es, ax
	read_file: read_file_from_disk

	; Loads the second-stage bootloader
	mov ax, BOOTLOADER_SEGMENT
	mov es, ax
	mov ds, ax
	jmp BOOTLOADER_SEGMENT:0x00

	
%include "utility-inl.asm"

root_directory_size: dw 0
root_directory_sector: dw 0
file_starting_cluster_number: dw 0
bootloader_cluster: dw 0
	
end:
	times 510 - ($ - $$) db 0	; Pads with zero bytes
	db 0x55, 0xaa
