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

	; TODO(fkp): Check if the BIOS supports LBA addressing
	; Reads rest of the bootloader to 0x7e00
	; mov ax, 0x07e0
	; mov es, ax
	; xor di, di
	mov ax, 0
	mov es, ax
	mov di, 0x00
	mov eax, 129
	mov dl, [bootDriveNumber]
;	mov word [diskAddressPacket.sectorsToRead], 4
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

	; Waits for a key to be pressed
	xor ax, ax
	int 0x16

	jmp word 0xffff:0x0000

MAX_SECTORS_PER_READ: equ 127

%if 0
; void read_disk(eax start, es:di into, dl drive)
read_disk:
	MAX_SECTORS_PER_READ: equ 127

	.check_read_length:
		cmp word [diskAddressPacket.sectorsToRead], MAX_SECTORS_PER_READ
		jg .read_failed

	.initialise_dap:
		mov [diskAddressPacket.startLow], eax
		mov [diskAddressPacket.segmentToReadInto], es
		mov [diskAddressPacket.offsetToReadInto], di

		push ax
		push si
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
		mov si, diskErrorMessage
		call print_string
		call reboot
%endif

read_disk:
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
	mov ah, 0x01
	int 0x13

	cmp ah, 0x01
	je .here

	mov si, diskErrorMessage
	call print_string

.here:
	call reboot

; Data
bootDriveNumber: db 0
welcomeMessage: db "PandaOS", CR, LF, 0
rebootMessage: db "Reboot?", CR, LF, 0
diskErrorMessage: db "Failed to read disk!", CR, LF, 0

diskAddressPacket:
	.size: db 16
	.unused: db 0
	.sectorsToRead: dw 1
	.offsetToReadInto: dw 0
	.segmentToReadInto: dw 0
	.startLow: dd 0
	.startHigh: dd 0

end:
	times 504 - ($ - $$) db 0

	; NOTE(fkp): Keep at the end (magic)!
	; This will be filled in by the hard disk creator
	bootloaderNumberOfSectors: dw 0
	kernelStartSector: dw 0
	kernelNumberOfSectors: dw 0

	dw 0xaa55

; NOTE(fkp): From here on is just a test for the bootloader loading
times 540 db 0
testMessage: db "Test from extra bootloader.", CR, LF, 0

times 2048 - ($ - $$) db 0
