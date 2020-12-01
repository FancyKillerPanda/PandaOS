; ===== Date Created: 01 December, 2020 ===== 
org 0x7c00

start:
	jmp short main
	nop

biosParameterBlock:

main:
	

end:
	times 510 - ($ - $$) db 0
	dw 0xaa55
