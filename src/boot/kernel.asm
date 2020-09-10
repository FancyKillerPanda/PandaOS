; ===== Date Created: 10 September, 2020 ===== 
bits 32

	mov eax,		dword 0xB8000
	mov [eax     ], byte 'X'
	mov [eax + 1 ], byte 0x40
	mov [eax + 2 ], byte '8'
	mov [eax + 3 ], byte 0x40
	mov [eax + 4 ], byte '6'
	mov [eax + 5 ], byte 0x40
	mov [eax + 6 ], byte ' '
	mov [eax + 7 ], byte 0x40
	mov [eax + 8 ], byte 'H'
	mov [eax + 9 ], byte 0x40
	mov [eax + 10], byte 'E'
	mov [eax + 11], byte 0x40
	mov [eax + 12], byte 'L'
	mov [eax + 13], byte 0x40
	mov [eax + 14], byte 'L'
	mov [eax + 15], byte 0x40
	mov [eax + 16], byte 'O'
	mov [eax + 17], byte 0x40

	jmp $ 
