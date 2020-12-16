; ===== Date Created: 02 December, 2020 ===== 

; void clear_screen()
clear_screen:
	.clear:
		mov ax, 0x0700				; Entire screen
		mov bx, 0x07				; Colour
		xor cx, cx					; Top-left of screen is (0, 0)
		mov dx, 0x184f				; Screen size: 24 rows x 79 columns
		int 0x10

	.move_cursor:
		xor dx, dx
		mov ax, 0x02
		xor bh, bh
		int 0x10

	.cleanup:
		ret

; void print_string(ds:si string)
; This function will clobber si
print_string:
	.check_for_newline:
		cmp byte [ds:si], LF
		jne .print_char
		inc byte [numberOfLinesPrinted]

	.print_char:
		mov al, [ds:si]
		cmp al, 0
		je .done

		mov ah, 0x0e
		mov bh, 0
		int 0x10

		inc si
		jmp .check_for_newline

	.done:
		ret

; void reboot()
reboot:
	mov si, rebootMessage
	call print_string

	; Waits for a key to be pressed
	xor ax, ax
	int 0x16

	jmp word 0xffff:0x0000

; void read_disk(cx sector, al number-to-read, es:bx into)
read_disk:
	.read:
		push ax
		push bx
		call calculate_chs
		pop bx
		pop ax

		mov dl, [bootDriveNumber]
		mov ah, 0x02
		int 0x13

		jc .read_failed
		ret

	.read_failed:
		mov si, diskErrorMessage
		call print_string
		call reboot

; (ch cylinder, cl sector, dh head) calculate_chs(cx LBA-sector)
calculate_chs:
	sectorsPerTrack: equ 18
	headsPerCylinder: equ 2

	.calculate_sector:
		xor dx, dx
		mov ax, cx
		mov bx, sectorsPerTrack
		div bx					; LBA div/mod SPT
		inc dx
		mov [tempSector], dl

	.calculate_head:
		; ax already contains quotient of LBA / SPT
		xor dx, dx
		mov bx, headsPerCylinder
		div bx
		mov [tempHead], dl

	.calculate_cylinder:
		xor dx, dx
		mov ax, cx
		mov bx, sectorsPerTrack * headsPerCylinder
		div bx
		mov [tempCylinder], ax

	.finish:
		; cx:       -- CH -- -- CL --
		; Cylinder: 76543210 98
		; Sector:              543210
		movzx cx, byte [tempSector]
		mov ax, word [tempCylinder]
		shl ax, 8
		or cx, ax
		mov ax, word [tempCylinder]
		and ax, 0xc000
		shr ax, 8
		or cx, ax
		mov dh, byte [tempHead]

		ret

tempCylinder: dw 0
tempHead: db 0
tempSector: db 0
