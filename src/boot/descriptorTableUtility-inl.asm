; ===== Date Created: 03 December, 2020 ===== 

; void describe_gdt()
describe_gdt:
	.setup:
		push es
		xor ax, ax
		mov es, ax
		mov di, [gdtEntry.pointer]

	.describe:
		; NULL descriptor
	    mov cx, 8
	    rep stosb

	    ; Code segment descriptor
	    mov [es:di],     word 0xffff ; Limit of 4GB
	    mov [es:di + 2], word 0x0000 ; Base
	    mov [es:di + 4], byte 0x00	; More base bits
	    mov [es:di + 5], byte 0x9a	; Access (present, executable, and readable are set)
	    mov [es:di + 6], byte 0xcf	; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00	; More base bits
	
	    add di, 8

	    ; Data segment descriptor
	    mov [es:di],     word 0xffff ; Limit of 4GB
	    mov [es:di + 2], word 0x0000 ; Base
	    mov [es:di + 4], byte 0x00	; More base bits
	    mov [es:di + 5], byte 0x92	; Access (present and writable are set)
	    mov [es:di + 6], byte 0xcf	; Flags (granularity and size) and limit
	    mov [es:di + 7], byte 0x00	; More base bits

	.cleanup:
		pop es

; void describe_idt()
describe_idt:
	.setup:
		push es
		xor ax, ax
		mov es, ax
		mov di, [gdtEntry.pointer]

	.describe:
		mov cx, 1024
		rep stosw

	.cleanup:
		pop es

; void load_descriptor_tables()
load_descriptor_tables:
	cli
	lgdt [gdtEntry]
	lidt [idtEntry]

gdtEntry:
	.size: dw 24
	.pointer: dd 0x7000

idtEntry:
	.size: dw 1024
	.pointer: dd 0x7100