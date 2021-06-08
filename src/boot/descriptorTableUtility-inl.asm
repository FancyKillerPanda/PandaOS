; ===== Date Created: 03 December, 2020 ===== 
bits 16

; void describe_gdt()
describe_gdt:
	.setup:
		push es
		xor ax, ax
		mov es, ax
		mov di, [gdtEntry.pointer]

	.describe:
		; NULL descriptor
		gdtNullOffset: equ 0
	    mov [es:di],     word 0x0000
	    mov [es:di + 2], word 0x0000
	    mov [es:di + 4], byte 0x00
	    mov [es:di + 5], byte 0x00
	    mov [es:di + 6], byte 0x00
	    mov [es:di + 7], byte 0x00

		add di, 8

	    ; Code segment descriptor (16-bit)
		gdtCode16Offset: equ 8
	    mov [es:di],     word 0xffff ; Limit of 4GB
	    mov [es:di + 2], word 0x0000 ; Base
	    mov [es:di + 4], byte 0x00	; More base bits
	    mov [es:di + 5], byte 0x98	; Access (present and executable are set)
	    mov [es:di + 6], byte 0x00	; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00	; More base bits
	
	    add di, 8

	    ; Data segment descriptor (16-bit)
		gdtData16Offset: equ 16
	    mov [es:di],     word 0xffff ; Limit of 4GB
	    mov [es:di + 2], word 0x0000 ; Base
	    mov [es:di + 4], byte 0x00	; More base bits
	    mov [es:di + 5], byte 0x92	; Access (present and writable are set)
	    mov [es:di + 6], byte 0x00	; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00	; More base bits

	    add di, 8

	    ; Code segment descriptor (32-bit)
		gdtCode32Offset: equ 24
	    mov [es:di],     word 0xffff ; Limit of 4GB
	    mov [es:di + 2], word 0x0000 ; Base
	    mov [es:di + 4], byte 0x00	; More base bits
	    mov [es:di + 5], byte 0x9a	; Access (present, executable, and readable are set)
	    mov [es:di + 6], byte 0xcf	; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00	; More base bits
	
	    add di, 8

	    ; Data segment descriptor (32-bit)
		gdtData32Offset: equ 32
	    mov [es:di],     word 0xffff ; Limit of 4GB
	    mov [es:di + 2], word 0x0000 ; Base
	    mov [es:di + 4], byte 0x00	; More base bits
	    mov [es:di + 5], byte 0x92	; Access (present and writable are set)
	    mov [es:di + 6], byte 0xcf	; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00	; More base bits

		add di, 8

	.cleanup:
		pop es
		ret

; void describe_idt()
describe_idt:
	.setup:
		push es
		xor ax, ax
		mov es, ax
		mov di, [idtEntry.pointer]

	.describe:
		mov cx, 1024
		rep stosw

	.cleanup:
		pop es
		ret

; void load_descriptor_tables()
load_descriptor_tables:
	cli
	lgdt [gdtEntry]
	lidt [idtEntry]

	ret

gdtEntry:
	.size: dw 40
	.pointer: dd 0x7000

idtEntry:
	.size: dw 1024
	.pointer: dd 0x7100

idtEntryRealMode:
	.size: dw 1024
	.pointer: dd 0x0000
