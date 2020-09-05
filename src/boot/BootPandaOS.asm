; ===== Date Created: 05 September, 2020 ===== 
bits 16							; We are working in 16-bit real mode
org 0x7c00						; Where all absolute addresses start

STAGE_TWO_LOAD_SEGMENT: equ 0x1000
CR: equ 0x0d
LF: equ 0x0a
	
main:
	; Two bytes (jmp) plus one byte (nop)
	; jmp short start				; Out of range...
	jmp start
	nop

boot_sector:
	; BIOS parameter block
	OEMName					db "PandaOS " ; TODO(fkp): Do we need a null-terminator?
	BytesPerSector			dw 512
	SectorsPerCluster		db 1
	ReservedSectors			dw 1
	FATTables				db 2
	RootDirectoryEntries	dw 224
	SectorsCount			dw 2880
	MediaDescriptor			db 0xf0
	SectorsPerFAT			dw 9
	SectorsPerTrack			dw 9
	HeadsCount				dw 2
	HiddenSectors			dd 0

	; Extended BIOS parameter block
	TotalSectorsCount		dd 0
	BootDriveNumber			db 0
	Reserved				db 0
	BootSignature			db 0x29
	VolumeID				dd 0
	VolumeLabel				db "PandaOSVolume"
	FilesystemType			db "FAT16   " ; TODO(fkp): Why the spaces?

print_string:
	lodsb						; Loads byte in si into al (and advances si)
	or al, al					; Checks if al is 0
	jz .done

	mov ah, 0x0e				; Interrupt subfunction to print the character
	mov bh, 0x00				; Default page
	mov bl, 0x00				; Default foreground colour
	int 0x10

	jmp print_string
.done:
	ret

failed_to_boot:
	mov si, disk_error_msg		; Loads the message
	call print_string
	call reboot

reboot:
	mov si, reboot_msg			; Loads the message
	call print_string

	; Waits for a key to be pressed
	xor ax, ax
	int 0x16

	jmp word 0xffff:0x0000

clear_screen:
	mov ah, 0x07				; Subfunction to clear the screen
	mov al, 0x00				; Clears the entire screen
	mov bh, 0x07				; Screen colours
	mov cx, 0x00				; Top-left of the screen is (0, 0)
	mov dh, 0x18				; Screen size: 24 rows
	mov dl, 0x4f				; Screen size: 79 cols

	int 0x10

	mov dx, 0x0000
	call move_cursor
	
	ret

move_cursor:
	; This function requires the location (row, col) in dx
	
	mov ah, 0x02				; Subfunction to move the cursor
	mov bh, 0x00				; Default page
	int 0x10

	ret

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
	mov si, loading_msg
	call print_string
	
	; Resets the disk system
	mov dl, [BootDriveNumber]
	xor ax, ax					; Interrupt subfunction 0
	int 0x13					; Calls the interrupt
								; It will set the carry flag on error
	jc failed_to_boot

	call reboot					; Just for now...

loading_msg: db "Loading PandaOS...", CR, LF, 0
disk_error_msg: db "Error: Failed to load disk!", CR, LF, 0
reboot_msg: db "Press any key to reboot...", CR, LF, 0

end:
	times 510 - ($ - $$) db 0	; Pads with zero bytes
	db 0x55, 0xaa
