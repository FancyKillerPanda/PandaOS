; ===== Date Created: 05 October, 2020 ===== 
bits 32

global load_page_directory
load_page_directory:
	push ebp
	mov ebp, esp
	
	; Sets the cr3 register to the location of the page directory (its
	; location would have been pushed onto the stack)
	mov eax, [esp + 8]
	mov cr3, eax
	
	mov esp, ebp
	pop ebp

	ret

global enable_paging
enable_paging:
	push ebp
	mov ebp, esp

	; Sets the 32nd bit in the cr0 register
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	mov esp, ebp
	pop ebp

	ret
