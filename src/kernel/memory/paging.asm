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

; void enable_paging()
global enable_paging
enable_paging:
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
