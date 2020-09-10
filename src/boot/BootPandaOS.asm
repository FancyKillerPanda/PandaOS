; ===== Date Created: 05 September, 2020 ===== 
bits 16							; We are working in 16-bit real mode
org 0x7c00						; Where all absolute addresses start

main:
	; Two bytes (jmp) plus one byte (nop)
	jmp short start
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
	print loading_msg
	
	; Resets the disk system
	mov dl, [BootDriveNumber]
	call reset_disk_system
	jc boot_failed

	; Finds the second-stage bootloader file
	find_file_on_disk kernel_loader_file, KERNEL_LOADER_SEGMENT
	mov [kernel_loader_cluster], ax

	load_fat

	; Reads the second-stage bootloader file
	read_file_from_disk [kernel_loader_cluster], KERNEL_LOADER_SEGMENT

	; Loads the second-stage bootloader
	mov ax, KERNEL_LOADER_SEGMENT
	mov es, ax
	mov ds, ax
	jmp KERNEL_LOADER_SEGMENT:0x00

	
%include "commonUtility-inl.asm"

root_directory_size: dw 0
root_directory_sector: dw 0
kernel_loader_cluster: dw 0
	
end:
	times 510 - ($ - $$) db 0	; Pads with zero bytes
	db 0x55, 0xaa
