; ===== Date Created: 02 December, 2020 ===== 
bits 16

%macro finish_if_a20_enabled 0
	call check_a20
	cmp ax, 0
	jne .success
%endmacro

; void try_enable_a20()
try_enable_a20:
	.try:
		finish_if_a20_enabled
		call try_set_a20_bios
		finish_if_a20_enabled
		call try_set_a20_keyboard
		finish_if_a20_enabled
		call try_set_a20_fast
		finish_if_a20_enabled

	.fail:
		mov si, a20FailedMessage
		call print_string
		call reboot

	.success:
		mov si, a20SuccessMessage
		call print_string
		ret

; void try_set_a20_bios()
try_set_a20_bios:
	mov ax, 0x2401
	int 0x15

; void try_set_a20_keyboard()
try_set_a20_keyboard:
	; Disables interrupts
	cli

	; Disables the keyboard
	call wait_for_8042_command
	mov al, 0xad
	out 0x64, al

	; Asks for input
	call wait_for_8042_command
	mov al, 0xd0
	out 0x64, al

	; Reads the input
	call wait_for_8042_data
	in al, 0x60
	push eax

	; Writes to the output
	call wait_for_8042_command
	mov al, 0xd1
	out 0x64, al
	
	; Write input back, with bit 2 set
	call wait_for_8042_command
	pop eax
	or al, 2
	out 0x60, al

	; Re-enables the keyboard
	call wait_for_8042_command
	mov al, 0xae
	out 0x64, al

	call wait_for_8042_command
	sti

; void try_set_a20_fast()
try_set_a20_fast:
	; Reads, sets bit 2, writes
	in al, 0x92
	or al, 2
	out 0x92, al

; void wait_for_8042_command()
wait_for_8042_command:
	in al, 0x64					; Gets input from a port
	test al, 2					; Checks if it's ready
	jnz wait_for_8042_command

	ret

; void wait_for_8042_data()
wait_for_8042_data:
	in al, 0x64					; Gets input from a port
	test al, 1					; Checks if it's ready
	jnz wait_for_8042_data

	ret

; ax check_a20()
check_a20:
	.setup:
		; Saves registers that will be overwritten
		pushf
		push ds
		push es
		push di
		push si

		; Disables interrupts
		cli

		; Sets es:di and ds:si
		xor ax, ax
		mov es, ax
		mov di, 0x0500

		mov ax, 0xffff
		mov ds, ax
		mov si, 0x0510

		; Saves bytes that we need to restore after the test
		mov al, [es:di]
		push ax
		mov al, [ds:si]
		push ax

	.conduct:
		; Conducts the test
		mov byte [es:di], 0x00
		mov byte [ds:si], 0xff
		cmp byte [es:di], 0xff

	.cleanup:
		; Restores bytes that were there before
		pop ax
		mov [ds:si], al
		pop ax
		mov [es:di], al

		; Returns 0 if memory wrapped around, 1 otherwise
		mov ax, 0
		je .done
		mov ax, 1

	.done:
		pop si
		pop di
		pop es
		pop ds
		popf

		ret
