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
		mov word [edidResults.pixelWidth], 1024
		mov word [edidResults.pixelHeight], 768
		jmp .get_edid_info_finished

	.edid_supported:
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
	; TODO(fkp): Implement
	ret

; void select_vesa_mode()
select_vesa_mode:
	; TODO(fkp): Implement
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
	
	.frameBuffer: dd 0
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
vesaBiosSupportedMessage: db "Info: VESA BIOS is supported.", CR, LF, 0
vesaBiosNotSupportedMessage: db "Error: VESA BIOS is not supported.", CR, LF, 0
vesaModeFoundMessage: db "Info: VESA mode found!", CR, LF, 0
vesaModeNotFoundMessage: db "Error: No VESA mode found!", CR, LF, 0
vesaSetModeFailedMessage: db "Error: Failed to set VESA mode!", CR, LF, 0
