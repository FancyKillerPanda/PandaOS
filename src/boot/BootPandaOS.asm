; ===== Date Created: 03 September, 2020 ===== 
	bits 16							; We are working in 16bit real mode

	;; Where the data segment should begin
	mov ax, 0x7c0
	mov ds, ax

	;; Where the stack should begin. It is placed directly after the
	;; 512 bytes of the bootloader.
	mov ax, 0x7e0
	mov ss, ax
	
	;; Creates an 8kB stack
	mov sp, 0x2000

	;; Program to clear screen and print message
	call clear_screen

	push 0x0000						; So that we move to (0, 0)
	call move_cursor
	add sp, 2						; Adds to the stack pointer

	push msg						; Pushes our message as a paremter
	call print_string
	add sp, 2						; Adds to the stack pointer

	;; Stops the program (clears interrupts and halts)
	cli
	hlt
	
msg:	db "Hello, world!", 0

clear_screen:
	;; Pushes all general registers onto the stack.
	;; It also saves the base pointer as the current stack pointer
	push bp
	mov bp, sp
	pusha

	mov ah, 0x07					; To clear the screen
	mov al, 0x00					; Clears the entire window
	mov bh, 0x07					; Screen colours
	mov cx, 0x00					; Top-left of screen is (0, 0)
	mov dh, 0x18					; Screen size: 24 rows
	mov dl, 0x4f					; Screen size: 79 cols
	int 0x10						; Calls the interrupt

	;; Pops all general registers off the stack.
	;; Resets the stack pointer to what it was
	popa
	mov sp, bp
	pop bp

	ret

move_cursor:
	;; Pushes all general registers onto the stack.
	;; It also saves the base pointer as the current stack pointer
	push bp
	mov bp, sp
	pusha

	;; Gets the argument from the stack.
	;; The contents of `bp` take up two bytes, and the data we want
	;; also takes up two bytes (total of four bytes offset)
	mov dx, [bp + 4]
	mov ah, 0x02					; To move the cursor
	mov bh, 0x00					; Default page
	
	int 0x10						; Calls the interrupt

	;; Pops all general registers off the stack.
	;; Resets the stack pointer to what it was
	popa
	mov sp, bp
	pop bp

	ret

print_string:
	;; Pushes all general registers onto the stack.
	;; It also saves the base pointer as the current stack pointer
	push bp
	mov bp, sp
	pusha

	mov si, [bp + 4]				; Gets a pointer to the data
	mov bh, 0x00					; Default page
	mov bl, 0x00					; Default foreground colour
	mov ah, 0x0e					; To print the character

.char:
	mov al, [si]					; Gets the char from the current pointer position
	add si, 1
	or al, 0						; Like `cmp`
	je .return
	int 0x10						; Calls the interrupt
	jmp .char
	
.return:
	;; Pops all general registers off the stack.
	;; Resets the stack pointer to what it was
	popa
	mov sp, bp
	pop bp

	ret

end:	
	times 510 - ($ - $$) db 0 		; Defines 510 zero bytes
	db 0x55, 0xaa
