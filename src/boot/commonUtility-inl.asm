; ===== Date Created: 07 September, 2020 ===== 

%ifndef COMMON_UTILITY_INL_ASM
%define COMMON_UTILITY_INL_ASM
	
%include "macros-inl.asm"
	
%ifndef UTILITY_NO_PRINT_STRING
; void print_string(ds:si text)
print_string:
	lodsb						; Loads byte in si into al (and advances si)
	or al, al					; Checks if al is 0
	jz .done

	mov ah, 0x0e				; Interrupt subfunction to print the character
	mov bx, 0x0000				; Default page and foreground colour
	int 0x10

	jmp print_string
	
.done:
	ret
%endif


%ifndef UTILITY_NO_CLEAR_SCREEN
; void clear_screen()
clear_screen:
	mov ax, 0x0700				; To clear the entire screen
	mov bh, 0x07				; Screen colours
	mov cx, 0x00				; Top-left of the screen is (0, 0)
	mov dx, 0x184f				; Screen size: 24 rows x 79 columns

	int 0x10

	; Moves the cursor to the top-left
	mov dx, 0x0000
	; call move_cursor
	mov ah, 0x02
	mov bh, 0x00
	int 0x10
	
	ret
%endif

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
	

%ifndef UTILITY_NO_BOOT_FAILED
; void boot_failed()
boot_failed:
	print diskErrorMessage
	call reboot
%endif
	
%ifndef UTILITY_NO_REBOOT
; void reboot()
reboot:
	print rebootMessage

	; Waits for a key to be pressed
	xor ax, ax
	int 0x16

	jmp word 0xffff:0x0000
%endif
	
%ifndef UTILITY_NO_RESET_DISK_SYSTEM
; void reset_disk_system(dl disk)
reset_disk_system:
	xor ax, ax					; Interrupt subfunction 0
	int 0x13					; Calls the interrupt
								; It will set the carry flag on error
	ret
%endif
	

%ifndef UTILITY_NO_READ_SECTOR
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
	mov bx, [sectorsPerTrack]
	xor dx, dx
	div bx						; Quotient: ax (LBA / sectors per track)
	; Remainder: dx (LBA % sectors per track)
	inc dx
	mov cl, dl

	mov bx, [headsCount]
	xor dx, dx
	div bx						; Quotient: ax (cylinder)
	; Remainder: dx (head)
	mov ch, al					; ch is the cylinder
	xchg dl, dh					; dh is the head number

	; Calls the interrupt to read the sector
	; mov ah, 0x02				; Subfunction two
	; mov al, 0x01				; Reads one sector
	mov ax, 0x0201
	mov dl, [bootDriveNumber]
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
%endif
	
	
%ifndef UTILITY_NO_EXTENDED_READ_LBA
; eax: start location (LBA)
; es: segment to read into
; di: offset to read into
; dl: boot drive number
extended_read_lba:
	push ax
	push si
	cmp [diskAddressPacket.sectorsToRead], word MAX_SECTORS_PER_READ
	jg .read_failed

.initialise_dap:
	mov [diskAddressPacket.startLow], eax
	mov [diskAddressPacket.segmentToReadInto], es
	mov [diskAddressPacket.offsetToReadInto], di
	mov ax, diskAddressPacket
	mov si, ax

.read:
	mov ah, 0x42
	int 0x13
	jc .read_failed

	pop si
	pop ax
	ret

.read_failed:
	print diskErrorMessage
	call reboot

diskAddressPacket:
	.size:					db 16
	.unused:				db 0
	.sectorsToRead:			dw 1
	.offsetToReadInto:		dw 0
	.segmentToReadInto:		dw 0
	.startLow:				dd 0
	.startHigh:				dd 0
%endif
	
	
%ifndef UTILITY_NO_DISKERRORMESSAGE
; diskErrorMessage: db "Error: Failed to load disk!", CR, LF, 0
diskErrorMessage: db "Disk error", CR, LF, 0
%endif

%ifndef UTILITY_NO_REBOOT
; rebootMessage: db "Press any key to reboot...", CR, LF, 0
rebootMessage: db "Reboot?", CR, LF, 0
%endif
	
rootDirectorySize: dw 0
rootDirectorySector: dw 0

%endif
