; ===== Date Created: 01 December, 2020 ===== 
org 0x7c00

CR: equ 0x0d
LF: equ 0x0a

start:
	jmp short main
	nop

; There is a short form of the FAT32 BPB
; biosParameterBlock: times 68 db 0
biosParameterBlock: times 87 db 0
	
main:
	mov [bootDriveNumber], dl

	mov si, welcomeMessage
	call print_string

	mov ax, 0x07e0
	mov es, ax
	xor bx, bx
	mov cl, 2					; Start sector (one-based)
	mov al, [bootloaderNumberOfExtraSectors]
	call read_disk

	mov si, testMessage
	call print_string

	jmp $

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

; Data
bootDriveNumber: db 0
welcomeMessage: db "PandaOS", CR, LF, 0
rebootMessage: db "Reboot?", CR, LF, 0
diskErrorMessage: db "Failed to read disk!", CR, LF, 0

end:
	times 504 - ($ - $$) db 0

	; NOTE(fkp): Keep at the end (magic)!
	; TODO(fkp): This should be filled in by the hard disk creator
	bootloaderNumberOfExtraSectors: dw 3
	kernelStartSector: dw 0
	kernelNumberOfSectors: dw 0

	dw 0xaa55

; NOTE(fkp): From here on is just a test for the bootloader loading
times 540 db 0
testMessage: db "Test from extra bootloader.", CR, LF, 0

times 2048 - ($ - $$) db 0
