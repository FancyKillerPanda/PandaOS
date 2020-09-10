; ===== Date Created: 07 September, 2020 ===== 
bits 16

%include "macros-inl.asm"
	
; TODO(fkp): Put these macros elsewhere
; void try_set_a20_bios()
%macro try_set_a20_bios 0
	mov ax, 0x2401
	int 0x15
%endmacro
	
; void try_set_a20_keyboard()
%macro try_set_a20_keyboard 0
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
%endmacro

; void try_set_a20_fast()
%macro try_set_a20_fast 0
	; Reads, sets bit 2, writes
	in al, 0x92
	or al, 2
	out 0x92, al
%endmacro
	
%macro is_a20_done 0
	call test_a20
	cmp ax, 0
	jne .done
%endmacro
	
%macro try_enable_a20 0
try_enable:
	is_a20_done
	try_set_a20_bios
	is_a20_done
	try_set_a20_keyboard
	is_a20_done
	try_set_a20_fast
	is_a20_done

.fail:
	print a20_failed_msg
	call reboot

.done:
	print a20_success_msg
%endmacro
	
	
main:
	print kernel_loader_msg

	; Global descriptor table set up at 0x0000:0x0800
	xor ax, ax
	mov es, ax
	mov di, 0x80

	; Tells the CPU where the GDT table is and its size
	lgdt [gdt_entry]
	lidt [idt_entry]

	; Tries to enable to A20 line
	try_enable_a20
	
hang:
	jmp hang

	
boot_failure:
	print disk_error_msg
	call reboot

	
gdt_start:
	; The NULL descriptor
	dq 0x0000000000000000

	; The code segment descriptor
	dw 0xffff					; Limit of 4GB
	dw 0x0000					; Base of zero
	db 0x00						; More base bits
	db 0x9a						; Access (present, executable, and readable are set)
	db 0xcf						; Flags (granularity and size) and limit
	db 0x00						; More base bits

	; The data segment descriptor
	dw 0xffff					; Limit of 4GB
	dw 0x0000					; Base of zero
	db 0x00						; More base bits
	db 0x92						; Access (present and writable are set)
	db 0xcf						; Flags (granularity and size) and limit
	db 0x00						; More base bits
gdt_end:

gdt_entry:
	dw gdt_end - gdt_start - 1
	dd gdt_start


idt_entry:
	dw 0x0800					; 2048 bytes (for 256 interrupts)
	dd 0x00000000				; Starts at location zero


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

	
	
%include "biosParameterBlock-inl.asm"
%include "utility-inl.asm"

kernel_loader_msg: db "Info: PandaOS kernel loader...", CR, LF, 0
a20_failed_msg: db "Error: Failed to enable A20 line!", CR, LF, 0
a20_success_msg: db "Info: Enabled A20 line!", CR, LF, 0
