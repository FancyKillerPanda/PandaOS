; ===== Date Created: 22 December, 2020 ===== 
bits 32

; void load_page_directory(PageDirectoryTable pageDirectoryTable)
global load_page_directory
load_page_directory:
	.setup:
		push ebp
		mov ebp, esp

	.load:
		; Sets the fourth control register to the location of the page
		; directory (its location has been pushed onto the stack).
		mov eax, [esp + 8]
		mov cr3, eax

	.cleanup:
		mov esp, ebp
		pop ebp

		ret

; void set_paging_bit_on_cpu()
global set_paging_bit_on_cpu
set_paging_bit_on_cpu:
	.setup:
		push ebp
		mov ebp, esp

	.enable:
		; Sets the 32nd bit in the first control register
		mov eax, cr0
		or eax, 0x80000000
		mov cr0, eax

	.cleanup:
		mov esp, ebp
		pop ebp

		ret

; TODO(fkp): Move these
PAGE_DIRECTORY: equ 0x2000
IDENTITY_PAGE_TABLE: equ 0x3000
KERNEL_PAGE_TABLE: equ 0x4000

PRESENT_FLAG: equ 0x01
READ_WRITE_FLAG: equ 0x02

NUMBER_OF_PAGE_DIRECTORY_ENTRIES: equ 1024
NUMBER_OF_PAGE_TABLE_ENTRIES: equ 1024

HIGHER_HALF_OFFSET: equ 0xc0000000

; void init_paging_structures()
init_paging_structures:
	.setup:
		push ebp
		mov ebp, esp

	; Initialises an empty page directory
	.page_directory:
		xor ecx, ecx

		.loop:
			mov dword [PAGE_DIRECTORY + ecx], READ_WRITE_FLAG

			; Loop for each four-byte entry in the directory
			add ecx, 4
			cmp ecx, NUMBER_OF_PAGE_DIRECTORY_ENTRIES * 4
			jl .loop

	.cleanup:
		mov esp, ebp
		pop ebp

		ret

; void identity_map_kernel()
identity_map_kernel:
	.setup:
		push ebp
		mov ebp, esp

	; TODO(fkp): Macros!
	.set_identity_page_table:
		xor ecx, ecx

		.loop_identity:
			; Offset by the page size each time
			mov eax, ecx
			mov ebx, 0x1000
			mul ebx
			or eax, PRESENT_FLAG | READ_WRITE_FLAG

			; Moves the entry into the table (each entry is four bytes)
			mov dword [IDENTITY_PAGE_TABLE + (ecx * 4)], eax

			; Loop for each four-byte entry in the directory
			inc ecx
			cmp ecx, NUMBER_OF_PAGE_TABLE_ENTRIES
			jl .loop_identity

	.set_kernel_page_table:
		xor ecx, ecx

		.loop_kernel:
			; Offset by the page size each time
			mov eax, ecx
			mov ebx, 0x1000
			mul ebx
			or eax, PRESENT_FLAG | READ_WRITE_FLAG

			; Moves the entry into the table (each entry is four bytes)
			mov dword [KERNEL_PAGE_TABLE + (ecx * 4)], eax

			; Loop for each four-byte entry in the directory
			inc ecx
			cmp ecx, NUMBER_OF_PAGE_TABLE_ENTRIES
			jl .loop_kernel

	.map:
		mov dword [PAGE_DIRECTORY], IDENTITY_PAGE_TABLE | PRESENT_FLAG | READ_WRITE_FLAG
		TABLE_INDEX: equ ((HIGHER_HALF_OFFSET / 1024) / 4096)
		TABLE_LOCATION: equ PAGE_DIRECTORY + (TABLE_INDEX * 4)
		mov dword [TABLE_LOCATION], KERNEL_PAGE_TABLE | PRESENT_FLAG | READ_WRITE_FLAG

		; Loads the address of the temporary paging directory
		mov eax, PAGE_DIRECTORY
		push eax
		call load_page_directory
		pop eax

		xchg bx, bx
		call set_paging_bit_on_cpu

	.cleanup:
		mov esp, ebp
		pop ebp

		ret
