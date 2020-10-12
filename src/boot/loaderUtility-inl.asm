; ===== Date Created: 10 September, 2020 ===== 

%ifndef LOADER_UTILITY_INL_ASM
%define LOADER_UTILITY_INL_ASM
	
; void wait_for_8042_command()
wait_for_8042_command:
	in al, 0x64					; Gets input from a port
	test al, 2					; Checks if it's ready
	jnz wait_for_8042_command
	ret

; void wait_for_8042_command()
wait_for_8042_data:
	in al, 0x64					; Gets input from a port
	test al, 1					; Checks if it's ready
	jnz wait_for_8042_command
	ret

; ax test_a20()
test_a20:
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

	; Conducts the test
	mov byte [es:di], 0x00
	mov byte [ds:si], 0xff
	cmp byte [es:di], 0xff

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

%endif
