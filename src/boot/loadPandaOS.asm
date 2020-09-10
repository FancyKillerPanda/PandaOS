; ===== Date Created: 07 September, 2020 ===== 
bits 16

main:
	mov si, second_stage_loader_msg
	call print_string

hang:
	jmp hang


boot_failure:
	mov si, disk_error_msg
	call print_string
	call reboot

%include "biosParameterBlock-inl.asm"
%include "macros-inl.asm"
%include "utility-inl.asm"

second_stage_loader_msg: db "PandaOS second-stage loader...", CR, LF, 0
	
end:
	; Some padding to make sure we can load more than one sector
	times 1000 db 0
