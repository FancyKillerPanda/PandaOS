; ===== Date Created: 22 December, 2020 ===== 
bits 32

PRESENT: equ 0x01
READ_WRITE: equ 0x02

NUMBER_OF_PAGE_DIRECTORY_ENTRIES: equ 1024
NUMBER_OF_PAGE_TABLE_ENTRIES: equ 1024

HIGHER_HALF_OFFSET: equ 0xc0000000

pageDirectory: equ 0x2000
identityPageTable: equ 0x3000
kernelPageTable: equ 0x4000

; TODO(fkp): Reevaluate globals
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

; void start_paging()
start_paging:
	.setup:
		push ebp
		mov ebp, esp

	; Loads the address of the temporary paging directory and
	; then tells the CPU to start paging
	.enable:
		mov eax, pageDirectory
		push eax
		call load_page_directory
		pop eax

		call set_paging_bit_on_cpu

	.cleanup:
		mov esp, ebp
		pop ebp

		ret

; void init_paging_structures()
init_paging_structures:
	.setup:
		push ebp
		mov ebp, esp

	; Initialises an empty page directory
	.page_directory:
		xor ecx, ecx

		.loop:
			; Each entry is four bytes. ecx holds the entry count
			mov dword [pageDirectory + (ecx * 4)], READ_WRITE

			; Loop for each entry in the directory
			inc ecx
			cmp ecx, NUMBER_OF_PAGE_DIRECTORY_ENTRIES
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

	.set_page_tables:
		xor ecx, ecx

		.loop:
			; Offset by the page size each time
			mov eax, ecx
			mov ebx, 0x1000
			mul ebx
			or eax, PRESENT | READ_WRITE

			; Moves the entry into the table (each entry is four bytes)
			mov dword [identityPageTable + (ecx * 4)], eax
			mov dword [kernelPageTable + (ecx * 4)], eax

			; Loop for each entry in the directory
			inc ecx
			cmp ecx, NUMBER_OF_PAGE_TABLE_ENTRIES
			jl .loop

	.map:
		mov dword [pageDirectory], identityPageTable | PRESENT | READ_WRITE

		TABLE_INDEX: equ ((HIGHER_HALF_OFFSET / 1024) / 4096)
		TABLE_LOCATION: equ pageDirectory + (TABLE_INDEX * 4)
		mov dword [TABLE_LOCATION], kernelPageTable | PRESENT | READ_WRITE

	.cleanup:
		mov esp, ebp
		pop ebp

		ret
