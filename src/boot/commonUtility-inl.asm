; ===== Date Created: 07 September, 2020 ===== 

%ifndef COMMON_UTILITY_INL_ASM
%define COMMON_UTILITY_INL_ASM
	
%include "macros-inl.asm"
	
; void print_string(ds:si text)
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


; void clear_screen()
clear_screen:
	; mov ah, 0x07				; Subfunction to clear the screen
	; mov al, 0x00				; Clears the entire screen
	mov ax, 0x0700
	mov bh, 0x07				; Screen colours
	mov cx, 0x00				; Top-left of the screen is (0, 0)
	; mov dh, 0x18				; Screen size: 24 rows
	; mov dl, 0x4f				; Screen size: 79 cols
	mov dx, 0x184f

	int 0x10

	; Moves the cursor to the top-left
	mov dx, 0x0000
	; call move_cursor
	mov ah, 0x02
	mov bh, 0x00
	int 0x10
	
	ret


; NOTE(fkp): This takes too much space. It has been
; inlined in the clear_screen function.
%if 0
; void move_cursor(dh row, dl col)
move_cursor:
	mov ah, 0x02				; Subfunction to move the cursor
	mov bh, 0x00				; Default page
	int 0x10
	ret
%endif
	

; void boot_failed()
boot_failed:
	print disk_error_msg
	call reboot

	
; void reboot()
reboot:
	print reboot_msg

	; Waits for a key to be pressed
	xor ax, ax
	int 0x16

	jmp word 0xffff:0x0000

	
; void reset_disk_system(dl disk)
reset_disk_system:
	xor ax, ax					; Interrupt subfunction 0
	int 0x13					; Calls the interrupt
								; It will set the carry flag on error
	ret

	
; void read_sector(es:bx into, ax LBA)
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
	; mov ah, 0x02				; Subfunction two
	; mov al, 0x01				; Reads one sector
	mov ax, 0x0201
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

	; Retry
	call reset_disk_system
	pop ax
	jmp .read

	
; loading_msg: db "Loading PandaOS...", CR, LF, 0
loading_msg: db "PandaOS", CR, LF, 0
; disk_error_msg: db "Error: Failed to load disk!", CR, LF, 0
disk_error_msg: db "Disk error", CR, LF, 0
; reboot_msg: db "Press any key to reboot...", CR, LF, 0
reboot_msg: db "Reboot?", CR, LF, 0
root_directory_size: dw 0
root_directory_sector: dw 0

%endif