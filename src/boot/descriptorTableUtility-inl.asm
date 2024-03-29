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
	    mov [es:di],     word 0xffff	; Limit of 4GB
	    mov [es:di + 2], word 0x0000	; Base
	    mov [es:di + 4], byte 0x00		; More base bits
	    mov [es:di + 5], byte 0x98		; Access (present and executable are set)
	    mov [es:di + 6], byte 0x00		; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00		; More base bits
	
	    add di, 8

	    ; Data segment descriptor (16-bit)
		gdtData16Offset: equ 16
	    mov [es:di],     word 0xffff	; Limit of 4GB
	    mov [es:di + 2], word 0x0000	; Base
	    mov [es:di + 4], byte 0x00		; More base bits
	    mov [es:di + 5], byte 0x92		; Access (present and writable are set)
	    mov [es:di + 6], byte 0x00		; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00		; More base bits

	    add di, 8

	    ; Code segment descriptor (32-bit ring 0)
		gdtCode32Offset: equ 24
	    mov [es:di],     word 0xffff	; Limit of 4GB
	    mov [es:di + 2], word 0x0000	; Base
	    mov [es:di + 4], byte 0x00		; More base bits
	    mov [es:di + 5], byte 0x9a		; Access (present, executable, and readable are set)
	    mov [es:di + 6], byte 0xcf		; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00		; More base bits
	
	    add di, 8

	    ; Data segment descriptor (32-bit ring 0)
		gdtData32Offset: equ 32
	    mov [es:di],     word 0xffff	; Limit of 4GB
	    mov [es:di + 2], word 0x0000	; Base
	    mov [es:di + 4], byte 0x00		; More base bits
	    mov [es:di + 5], byte 0x92		; Access (present and writable are set)
	    mov [es:di + 6], byte 0xcf		; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00		; More base bits

		add di, 8

	    ; Code segment descriptor (32-bit ring 3)
		gdtCode32Ring3Offset: equ 40
	    mov [es:di],     word 0xffff	; Limit of 4GB
	    mov [es:di + 2], word 0x0000	; Base
	    mov [es:di + 4], byte 0x00		; More base bits
	    mov [es:di + 5], byte 0xfa		; Access (present, executable, readable, and ring 3 are set)
	    mov [es:di + 6], byte 0xcf		; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00		; More base bits
	
	    add di, 8

	    ; Data segment descriptor (32-bit ring 3)
		gdtData32Ring3Offset: equ 48
	    mov [es:di],     word 0xffff	; Limit of 4GB
	    mov [es:di + 2], word 0x0000	; Base
	    mov [es:di + 4], byte 0x00		; More base bits
	    mov [es:di + 5], byte 0xf2		; Access (present, writable, and ring 3 are set)
	    mov [es:di + 6], byte 0xcf		; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00		; More base bits

		add di, 8

		gdtTssOffset: equ 56
		mov ax, word [tssEntry.size]
	    mov [es:di],     ax				; Limit (size of TSS entry)
		mov eax, dword [tssEntry.pointer]
	    mov [es:di + 2], ax				; Base
		shr eax, 16
	    mov [es:di + 4], al				; More base bits
	    mov [es:di + 5], byte 0xe9		; Access (present, executable, and ring 3 are set)
	    mov [es:di + 6], byte 0x00		; Flags (granularity and size) and limit
		shr eax, 8
	    mov [es:di + 7], al				; More base bits

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
		mov cx, [idtEntry.size]
		rep stosb

	.cleanup:
		pop es
		ret

; void describe_tss()
describe_tss:
	.setup:
		push es
		xor ax, ax
		mov es, ax
		mov di, [tssEntry.pointer] ; Since word, only gets bottom half

	.describe:
		mov cx, [tssEntry.size]
		rep stosb

		; The kernel stack segment
		mov di, [tssEntry.pointer]
		mov [es:di + 8], dword gdtData32Offset

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
	.size: dw 64
	.pointer: dd 0x7000

idtEntry:
	.size: dw 1024
	.pointer: dd 0x7100

idtEntryRealMode:
	.size: dw 1024
	.pointer: dd 0x0000

; This isn't actually needed, just for consistency
tssEntry:
	.size: dw 104
	.pointer: dd HIGHER_HALF_OFFSET + 0x7500
