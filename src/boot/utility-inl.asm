; ===== Date Created: 02 December, 2020 ===== 

; void print_string(ds:si string)
; This function will mangle si
print_string:
	.print_char:
		mov al, [ds:si]
		cmp al, 0
		je .done

		mov ah, 0x0e
		mov bh, 0
		int 0x10

		inc si
		jmp .print_char

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

; NOTE(fkp): This is currently limited to reading up to sector 63
; void read_disk(cl sector, al number to read, es:bx into)
read_disk:
	.read:
		mov dl, [bootDriveNumber]

		xor ch, ch
		xor dh, dh
		mov ah, 0x02
		int 0x13

		jc .read_failed
		ret

	.read_failed:
		mov si, diskErrorMessage
		call print_string
		call reboot
