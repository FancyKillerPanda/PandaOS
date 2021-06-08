; ===== Date Created: 08 December, 2020 ===== 
bits 16

MEMORY_MAP_LOCATION: equ 0x0500

; void get_memory_map()
get_memory_map:
	.first_read:
		mov ax, MEMORY_MAP_LOCATION / 16
		mov es, ax
		xor di, di
		xor ebx, ebx
		mov edx, 0x534d4150
		mov eax, 0xe820
		mov ecx, 24

		int 0x15
		jc .memory_map_not_detected
		cmp eax, 0x534d4150
		jne .memory_map_not_detected

	.get_next:
		inc dword [memoryMap.numberOfEntries]

		add di, 24
		mov eax, 0xe820
		mov cx, 24
		mov edx, 0x534d4150

		int 0x15
		jc .memory_map_finished
		cmp ebx, 0
		je .memory_map_finished

		jmp .get_next

	.memory_map_not_detected:
		mov si, memoryMapNotDetectedMessage
		call print_string
		call reboot

	.memory_map_finished:
		mov si, memoryMapFinishedMessage
		call print_string
		ret

SIZE_OF_MEMORY_MAP: equ 16
memoryMap:
	.pointer: dd MEMORY_MAP_LOCATION
	.numberOfEntries dd 0
