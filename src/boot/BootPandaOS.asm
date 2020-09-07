; ===== Date Created: 05 September, 2020 ===== 
bits 16							; We are working in 16-bit real mode
org 0x7c00						; Where all absolute addresses start

ENTRY_CLUSTER_NUMBER_OFFSET: equ 0x1a
DIRECTORY_LOAD_SEGMENT: equ 0x1000
MAX_READ_ATTEMPTS: equ 4
CR: equ 0x0d
LF: equ 0x0a
	
main:
	; Two bytes (jmp) plus one byte (nop)
	; jmp short start				; Out of range...
	jmp start
	nop

boot_sector:
	; BIOS parameter block
	OEMName					db "PandaOS "
	BytesPerSector			dw 512
	SectorsPerCluster		db 1
	ReservedSectors			dw 1
	FATCount				db 2
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
	VolumeLabel				db "PandaVolume"
	FilesystemType			db "FAT16   "

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

boot_failed:
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

read_sector:
	xor cx, cx					; Resets the "try" count

.read:
	; Stores the logical block, try count, and data buffer offset
	push ax
	push cx
	push bx

	; Cylinder = (LBA / sectors per track) / number of heads
	; Sector = (LBA % sectors per track) + 1
	; Head = (LBA / sectors per track) % number of heads
	mov bx, [SectorsPerTrack]
	xor dx, dx
	div bx						; Quotient: ax (LBA / sectors per track)
								; Remainder: dx (LBA % sectors per track)
	inc dx
	mov cl, dl

	mov bx, [HeadsCount]
	xor dx, dx
	div bx						; Quotient: ax (cylinder)
								; Remainder: dx (head)
	mov ch, al					; ch is the cylinder
	xchg dl, dh					; dh is the head number

	; Calls the interrupt to read the sector
	mov ah, 0x02				; Subfunction two
	mov al, 0x01				; Reads one sector
	mov dl, [BootDriveNumber]
	pop bx						; Restores the data buffer offset
	int 0x13
	jc .read_failed

	; On success...
	pop cx
	pop ax
	ret

.read_failed:
	; Check if we should try again (up to a certain number of tries)
	pop cx
	inc cx
	cmp cx, [MAX_READ_ATTEMPTS]
	je boot_failed

	; Reset the disk system
	xor ax, ax
	int 0x13

	; Retry
	pop ax
	jmp .read

read_next_sector:
	push cx
	push ax
	xor bx, bx
	call read_sector

check_next_entry:
	mov cx, 11					; Filenames are eleven bytes long
	mov di, bx					; Address of directory entry
	; lea si, filename			; TODO(fkp): Don't need this?
	repz cmpsb					; Compares the filename to memory
	je found_file

	add bx, word 32				; Entries are 32 bytes, move to the next one
	cmp bx, word [BytesPerSector]
	jne check_next_entry

	pop ax
	inc ax						; Check the next sector next time
	pop cx
	loopnz read_next_sector		; Try find another sector
	jmp boot_failed

found_file:
	; bx is still pointing to the start of the entry
	mov ax, [es:bx + ENTRY_CLUSTER_NUMBER_OFFSET]
	mov [file_starting_cluster_number], ax

load_fat:
	mov ax, DIRECTORY_LOAD_SEGMENT
	mov es, ax

	; Calculates the offset of the FAT
	mov ax, word [ReservedSectors]
	add ax, word [HiddenSectors]
	adc ax, word [HiddenSectors + 2]

	; Read all the FAT sectors into memory
	mov cx, word [SectorsPerFAT]
	xor bx, bx

.read_next_fat_sector:
	push cx
	push ax
	call read_sector

	pop cx
	pop ax
	inc ax

	add bx, word [BytesPerSector]
	loopnz .read_next_fat_sector ; Continues with the next sector

read_file_from_disk:
	; Sets the segment that will receive the file
	mov ax, DIRECTORY_LOAD_SEGMENT
	mov es, ax
	xor bx, bx					; Memory offset of loading = 0

	; The start of the FAT
	mov cx, [file_starting_cluster_number]

.read_next_sector:
	; Locates the sector
	; Sector to read = current FAT entry + root directory - 2
	mov ax, cx
	add ax, [root_directory_sector]
	add ax, [root_directory_size]
	sub ax, 2

	; Reads the sector
	push cx
	call read_sector
	pop cx
	add bx, [BytesPerSector]	; Moves pointer to the next sector

	; Gets the next sector from the FAT
	push ds
	mov dx, DIRECTORY_LOAD_SEGMENT
	mov ds, dx

	mov si, cx					; Source index is the current FAT entry
	mov dx, cx					; Offset = entry * 1.5 bytes
	shr dx, 1					; Shifts right
	add si, dx

	mov dx, [ds:si]				; Reads the entry from memory
	test cx, 1					; Checks which way to shift
	jnz .read_next_cluster_odd
	and dx, 0x0fff				; Masks out the top four bits
	jmp .read_next

.read_next_cluster_odd:
	shr dx, 4					; Shifts the new cluster to the right

.read_next:
	pop ds						; Restores to the normal data segment
	mov cx, dx					; Stores the new FAT entry in cx
	cmp cx, 0x0ff8				; Magic value signalling that this is the last segment
	jl .read_next_sector
	
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
	jc boot_failed

	; Calculates the number of sectors the root directory occupies
	; (number of sectors in root directory * 32) / 512
	mov ax, 32
	xor dx, dx
	mul word [RootDirectoryEntries]
	div word [BytesPerSector]
	mov cx, ax
	mov [root_directory_size], cx

	; Calculates the starting sector of the root directory
	; (number of FATs * sectors per FAT) + number of hidden and reserved sectors
	xor ax, ax
	mov al, [FATCount]
	mov bx, [SectorsPerFAT]
	mul bx
	add ax, word [HiddenSectors]
	add ax, word [HiddenSectors + 2]
	add ax, word [ReservedSectors]
	mov [root_directory_sector], ax

	call reboot					; Just for now...

loading_msg: db "Loading PandaOS...", CR, LF, 0
disk_error_msg: db "Error: Failed to load disk!", CR, LF, 0
reboot_msg: db "Press any key to reboot...", CR, LF, 0

root_directory_size: dw 0
root_directory_sector: dw 0
file_starting_cluster_number: dw 0
	
end:
	times 510 - ($ - $$) db 0	; Pads with zero bytes
	db 0x55, 0xaa
