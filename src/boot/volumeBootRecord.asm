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
	mov si, welcomeMessage
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

; Data
welcomeMessage: db "PandaOS", CR, LF, 0

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
