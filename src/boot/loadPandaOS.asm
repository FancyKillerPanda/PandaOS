; ===== Date Created: 07 September, 2020 ===== 
bits 16

%include "macros-inl.asm"
	
main:
	print kernel_loader_msg

	; Copies the boot sector
	push ds						; Original bootsector is at 0x0000:0x7c00
	xor ax, ax
	mov ds, ax
	mov si, 0x7c03				; Actual data starts at 0x7c03
	mov di, bios_parameter_block
	mov cx, 34					; The size of the boot sector
	rep movsb
	pop ds
	
	; Locates and reads the kernel file
	find_file_on_disk kernel_file, KERNEL_SEGMENT
	mov [kernel_file_cluster], ax
	read_file_from_disk [kernel_file_cluster], KERNEL_SEGMENT

	; Resets the disk system
	mov dl, [BootDriveNumber]
	call reset_disk_system
	
	; Tries to enable to A20 line
	try_enable_a20

	; Global descriptor table set up at 0x0000:0x0800
	xor ax, ax
	mov es, ax
	mov di, ax

	; Interrupt descriptor table set up with empty bytes
	mov cx, 2048
	rep stosb					; Gives 256 empty IDT entries

	; NULL descriptor
    mov cx, 4
    rep stosw

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

	; Disables interrupt
	cli
	
	; Tells the CPU where the GDT table is and its size
	lgdt [gdt_entry]
	lidt [idt_entry]
	
	; Enables protected mode
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	; Clears the prefetch queue
	jmp .clear_prefetch_queue
	nop
	nop
.clear_prefetch_queue:

	; Sets up the segments
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x00030000			; Stack grows downwards from 0x00030000

	; Jumps to the kernel
	db 0x66
	db 0xea
	dd KERNEL_FLAT_ADDRESS
	dw 0x0008

	; Shouldn't get here
	call boot_failed
	
gdt_entry:
	dw 24
	dd 2048

idt_entry:
	dw 2048
	dd 0
	
	
%include "biosParameterBlock-inl.asm"
%include "commonUtility-inl.asm"
%include "loaderUtility-inl.asm"

kernel_loader_msg: db "Info: PandaOS kernel loader...", CR, LF, 0
a20_failed_msg: db "Error: Failed to enable A20 line!", CR, LF, 0
a20_success_msg: db "Info: Enabled A20 line!", CR, LF, 0
jumping_msg: db "Info: Jumping to the kernel!", CR, LF, 0

kernel_file: db "pKernelAbin"
kernel_file_cluster: dw 0
