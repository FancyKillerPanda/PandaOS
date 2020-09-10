; ===== Date Created: 07 September, 2020 ===== 
bits 16

%include "macros-inl.asm"
	
main:
	print kernel_loader_msg

hang:
	jmp hang


boot_failure:
	print disk_error_msg
	call reboot

%include "biosParameterBlock-inl.asm"
%include "utility-inl.asm"

kernel_loader_msg: db "PandaOS kernel loader...", CR, LF, 0
	
end:
	; Some padding to make sure we can load more than one sector
	times 1000 db 0
