; ===== Date Created: 03 September, 2020 ===== 
	jmp $
	times 510 - ($ - $$) db 0 		; Defines 510 zero bytes

	db 0x55, 0xaa
