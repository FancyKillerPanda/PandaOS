; ===== Date Created: 07 December, 2020 ===== 
bits 16

HIGHER_HALF_OFFSET: equ 0xc0000000
KERNEL_FLAT_ADDRESS: equ 0x00100000

; void load_kernel()
load_kernel:
	.setup:
		mov si, loadingKernelMessage
		call print_string

		; Temporary buffer will be 32 sectors at 0x2000
		tempBufferSegment: equ 0x0200
		maxSectorsPerRead: equ 32

		sectorsAlreadyRead: dw 0
		numberOfSectorsToReadNext: dw 0

	.calculate_number_of_sectors:
		mov dx, word [kernelNumberOfSectors]
		sub dx, word [sectorsAlreadyRead]
		cmp dx, maxSectorsPerRead
		jle .do_read
		mov dx, maxSectorsPerRead

	.do_read:
		mov [numberOfSectorsToReadNext], dx

		; Sets es:bx to be destination
		mov dx, tempBufferSegment
		mov es, dx
		xor bx, bx
		
		mov cx, word [kernelStartSector]
		add cx, word [sectorsAlreadyRead]
		mov ax, word [numberOfSectorsToReadNext]
		call read_disk

	.copy_to_real_location:
		enable_protected_mode

		; Source location
		mov eax, tempBufferSegment
		mov ecx, 0x10
		mul ecx
		mov esi, eax

		; Destination location
		movzx eax, word [sectorsAlreadyRead]
		mov ecx, 512
		mul ecx
		mov edx, KERNEL_FLAT_ADDRESS
		add edx, eax
		mov edi, edx

		; We are moving ((512 / 4) * number of sectors) DWORDs
		mov eax, 128
		movzx ecx, word [numberOfSectorsToReadNext]
		mul ecx
		mov ecx, eax

		rep movsd
		enable_real_mode

	.read_again_or_finish:
		mov ax, word [sectorsAlreadyRead]
		add ax, word [numberOfSectorsToReadNext]
		mov word [sectorsAlreadyRead], ax

		cmp ax, word [kernelNumberOfSectors]
		jl .calculate_number_of_sectors

	.done:
		mov si, loadedKernelMessage
		call print_string
		ret

loadingKernelMessage: db "Info: Loading kernel...", CR, LF, 0
loadedKernelMessage: db "Info: Loaded kernel!", CR, LF, 0
