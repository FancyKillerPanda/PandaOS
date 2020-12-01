; ===== Date Created: 01 December, 2020 ===== 
org 0x7c00

CR: equ 0x0d
LF: equ 0x0a

start:
	jmp short main
	nop

main:
	mov si, testString
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
testString: db "Hello, world!", CR, LF, 0

end:
	times 510 - ($ - $$) db 0
	dw 0xaa55
