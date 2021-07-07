; ===== Date Created: 07 July, 2021 ===== 
bits 16

; void get_edid_info()
get_edid_info:
	.setup:
		push es
		mov ax, ds
		mov es, ax
		mov di, edidInfo
	
	.call_bios_interrupt:
		mov ax, 0x4f15
		mov bl, 0x01
		xor cx, cx
		xor dx, dx
		int 0x10

		; Checks that it worked
		cmp ax, 0x004f
		je .edid_supported

	.edid_not_supported:
		mov si, edidBiosNotSupportedMessage
		call print_string

		mov word [edidResults.pixelWidth], 1024
		mov word [edidResults.pixelHeight], 768
		jmp .get_edid_info_finished

	.edid_supported:
		mov si, edidBiosSupportedMessage
		call print_string

		; Calculates the width
		movzx ax, byte [edidInfo.widthLower8]
		movzx bx, byte [edidInfo.widthUpper4]
		and bl, 0xf0
		shl bx, 4
		add ax, bx
		mov [edidResults.pixelWidth], ax

		; Calculates the height
		movzx ax, byte [edidInfo.heightLower8]
		movzx bx, byte [edidInfo.heightUpper4]
		and bl, 0xf0
		shl bx, 4
		add ax, bx
		mov [edidResults.pixelHeight], ax

	.get_edid_info_finished:
		pop es
		ret

; void get_vesa_bios_info()
get_vesa_bios_info:
	.setup:
		push es
		mov ax, 0x1000
		mov es, ax
		mov di, vbeInfo

	.call_bios_interrupt:
		mov ax, 0x4f00
		int 0x10
		pop es
	
		cmp ax, 0x004f
		je .vesa_supported
	
	.vesa_not_supported:
		; mov si, vesaBiosNotSupportedMessage
		; call print_string
		call reboot

	.vesa_supported:
		; mov si, vesaBiosSupportedMessage
		; call print_string

		ret

; void select_vesa_mode()
select_vesa_mode:
	.setup:
		push es
		xor ax, ax
		mov es, ax

		mov eax, [vbeInfo.videoModes]
		mov si, ax
		shr eax, 16
		mov fs, ax

		; This can be checked later to make sure we've actually set it
		mov word [bestVESAMode], 0
		mov word [bestVESAWidth], 0
		mov word [bestVESAHeight], 0
		mov byte [bestVESABitsPerPixel], 0
		mov word [bestVESAPitch], 0
		mov dword [bestVESAFrameBuffer], 0

	.next_mode:
		cmp word [fs:si], 0xffff
		je .end_of_mode_list

		push es
		mov ax, 0x4f01
		mov cx, word [fs:si]
		mov di, vbeModeInfo
		int 0x10
		pop es

		cmp ax, 0x004f
		jne .go_to_next_mode

	.check_width_and_height:
		mov ax, [edidResults.pixelWidth]
		cmp word [es:vbeModeInfo.width], ax
		jg .go_to_next_mode
		mov ax, [edidResults.pixelHeight]
		cmp word [es:vbeModeInfo.height], ax
		jg .go_to_next_mode

	.check_bits_per_pixel:
		cmp byte [es:vbeModeInfo.bitsPerPixel], 24
		jl .go_to_next_mode

	.check_memory_model:
		; Checks for correct memory model (direct colour)
		cmp byte [es:vbeModeInfo.memoryModel], 0x06
		jne .go_to_next_mode

	.check_masks:
		cmp byte [es:vbeModeInfo.redMask], 8
		jne .go_to_next_mode
		cmp byte [es:vbeModeInfo.greenMask], 8
		jne .go_to_next_mode
		cmp byte [es:vbeModeInfo.blueMask], 8
		jne .go_to_next_mode

	.check_positions:
		cmp byte [es:vbeModeInfo.redPosition], 16
		jne .go_to_next_mode
		cmp byte [es:vbeModeInfo.greenPosition], 8
		jne .go_to_next_mode
		cmp byte [es:vbeModeInfo.bluePosition], 0
		jne .go_to_next_mode

	.check_32_bits_per_pixel:
		; If we're only 24bpp, we can continue
		cmp byte [es:vbeModeInfo.bitsPerPixel], 32
		jg .go_to_next_mode
		jl .skip_32_bits_per_pixel_check

		; Checks mask and position of the reserved bits
		cmp byte [es:vbeModeInfo.reservedMask], 8
		jne .go_to_next_mode
		cmp byte [es:vbeModeInfo.reservedPosition], 24
		jne .go_to_next_mode

	.skip_32_bits_per_pixel_check:
		; Compares with the current best mode
		mov ax, [bestVESAWidth]
		cmp [es:vbeModeInfo.width], ax
		jl .go_to_next_mode
		mov ax, [bestVESAHeight]
		cmp [es:vbeModeInfo.height], ax
		jl .go_to_next_mode
		mov al, [bestVESABitsPerPixel]
		cmp [es:vbeModeInfo.bitsPerPixel], al
		jl .go_to_next_mode

	.set_values:
		; We are the best mode so far, set the values
		mov ax, [fs:si]
		mov word [bestVESAMode], ax
		mov ax, [es:vbeModeInfo.width]
		mov word [bestVESAWidth], ax
		mov ax, [es:vbeModeInfo.height]
		mov word [bestVESAHeight], ax
		mov al, [es:vbeModeInfo.bitsPerPixel]
		mov byte [bestVESABitsPerPixel], al
		mov ax, [es:vbeModeInfo.pitch]
		mov word [bestVESAPitch], ax
		mov eax, [es:vbeModeInfo.framebuffer]
		mov dword [bestVESAFrameBuffer], eax

	.go_to_next_mode:
		add si, 4
		jmp .next_mode

	.error:
		; mov si, vesaModeNotFoundMessage
		; call print_string
		call reboot

	.end_of_mode_list:
		; Errors out if we have not found a valid mode
		cmp word [bestVESAMode], 0
		je .error

	.mode_found:
		; mov si, vesaModeFoundMessage
		; call print_string

		mov ax, [bestVESAWidth]
		mov word [es:videoMode.screenWidth], ax
		mov ax, [bestVESAHeight]
		mov word [es:videoMode.screenHeight], ax
		mov al, [bestVESABitsPerPixel]
		mov byte [es:videoMode.bitsPerPixel], al
		mov ax, [bestVESAPitch]
		mov word [es:videoMode.pitch], ax
		mov eax, [bestVESAFrameBuffer]
		mov dword [es:videoMode.framebufferPointer], eax

		mov ax, [bestVESAMode]

	.finished:
		pop es
		ret

; void set_vesa_mode()
set_vesa_mode:
	; TODO(fkp): Implement
	ret

; NOTE(fkp): Here lies data
videoMode:
	.screenWidth: dw 0
	.screenHeight: dw 0
	.bitsPerPixel: db 0
	.pitch: dw 0
	.framebufferPointer: dd 0

vbeInfo:
	.signature: db "VBE2"
	.version: dw 0
	.oem: dd 0
	.capabilities: dd 0
	.videoModes: dd 0
	.videoMemorySize: dw 0
	.softwareRevision: dw 0
	.vendor: dd 0
	.productName: dd 0
	.productRevision: dd 0
	.reserved: times 222 db 0
	.oemData: times 256 db 0

vbeModeInfo:
	.attributes: dw 0
	.windowA: db 0
	.windowB: db 0
	.granularity: dw 0
	.windowSize: dw 0
	.segmentA: dw 0
	.segmentB: dw 0
	.windowFunctionPointer: dd 0
	.pitch: dw 0
	.width: dw 0
	.height: dw 0
	.wChar: db 0
	.yChar: db 0
	.planes: db 0
	.bitsPerPixel: db 0
	.banks: db 0
	.memoryModel: db 0
	.bankSize: db 0
	.imagePages: db 0
	.reserved0: db 0
	
	.redMask: db 0
	.redPosition: db 0
	.greenMask: db 0
	.greenPosition: db 0
	.blueMask: db 0
	.bluePosition: db 0
	.reservedMask: db 0
	.reservedPosition: db 0
	.directColourAttributes: db 0
	
	.framebuffer: dd 0
	.offScreenMemoryOffset: dd 0
	.offScreenMemorySize: dw 0
	
	.reserved1: times 206 db 0
	
edidResults:
	.pixelWidth: dw 0
	.pixelHeight: dw 0

edidInfo:
	; NOTE(fkp): The blank areas are not needed by us
	times 54 db 0
	
	; Here starts the detailed timing description
	times 2 db 0
	.widthLower8 db 0
	db 0
	.widthUpper4 db 0
	.heightLower8 db 0
	db 0
	.heightUpper4 db 0

	times 128 - ($ - edidInfo) db 0

; Variables for VESA mode selection
bestVESAMode: dw 0
bestVESAWidth: dw 0
bestVESAHeight: dw 0
bestVESABitsPerPixel: db 0
bestVESAPitch: dw 0
bestVESAFrameBuffer: dd 0

; Output strings
edidBiosSupportedMessage: db "Info: EDID info is supported.", CR, LF, 0
edidBiosNotSupportedMessage: db "Error: EDID info is not supported.", CR, LF, 0

vesaBiosSupportedMessage: db "Info: VESA BIOS is supported.", CR, LF, 0
vesaBiosNotSupportedMessage: db "Error: VESA BIOS is not supported.", CR, LF, 0
vesaModeFoundMessage: db "Info: VESA mode found!", CR, LF, 0
vesaModeNotFoundMessage: db "Error: No VESA mode found!", CR, LF, 0
vesaSetModeFailedMessage: db "Error: Failed to set VESA mode!", CR, LF, 0
