; ===== Date Created: 20 September, 2020 ===== 
bits 16
org 0x7c00

main:
	jmp $

end:
	times 510 - ($ - $$) db 0
	db 0x55, 0xaa
