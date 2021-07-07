; ===== Date Created: 07 July, 2021 ===== 
bits 16

; void get_edid_info()
get_edid_info:
	; TODO(fkp): Implement
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
