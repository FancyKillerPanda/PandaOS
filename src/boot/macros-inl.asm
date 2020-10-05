; ===== Date Created: 07 September, 2020 ===== 

%ifndef MACROS_INL_ASM
%define MACROS_INL_ASM

BYTES_PER_DIRECTORY_ENTRY: equ 32
ENTRY_CLUSTER_NUMBER_OFFSET: equ 0x1a
DIRECTORY_LOAD_SEGMENT: equ 0x1000
FAT_LOAD_SEGMENT: equ 0x0ee0
KERNEL_LOADER_SEGMENT: equ 0x1000
KERNEL_SEGMENT: equ 0x2000
KERNEL_FLAT_ADDRESS: equ 0x20000
MAX_READ_ATTEMPTS: equ 4
MAX_SECTORS_PER_READ: equ 127
	
CR: equ 0x0d
LF: equ 0x0a
	
; void print(text)
%macro print 1
	mov si, %1
	call print_string
%endmacro
	
; void log(text)
%macro log 1
	pusha
	print %1
	popa
%endmacro
	
; void load_fat()
%macro load_fat 0
	mov ax, FAT_LOAD_SEGMENT
	mov es, ax

	; Calculates the offset of the FAT
	mov ax, word [reservedSectors]
	add ax, word [hiddenSectors]
	adc ax, word [hiddenSectors + 2]

	; Read all the FAT sectors into memory
	mov cx, word [sectorsPerFAT]
	xor bx, bx

%%.read_next_fat_sector:
	push cx
	push ax
	call read_sector

	pop cx
	pop ax
	inc ax

	add bx, word [bytesPerSector]
	loopnz %%.read_next_fat_sector ; Continues with the next sector
%endmacro

	
; ax find_file_on_disk(filename, read_segment)
%macro find_file_on_disk 2
	mov ax, %2
	mov es, ax
	
	; Calculates the number of sectors the root directory occupies
	; (number of sectors in root directory * 32) / 512
	mov ax, BYTES_PER_DIRECTORY_ENTRY
	xor dx, dx
	mul word [rootDirectoryEntries]
	div word [bytesPerSector]
	mov cx, ax
	mov [rootDirectorySize], cx

	; Calculates the starting sector of the root directory
	; (number of FATs * sectors per FAT) + number of hidden and reserved sectors
	xor ax, ax
	mov al, [FATCount]
	mov bx, [sectorsPerFAT]
	mul bx
	add ax, word [hiddenSectors]
	adc ax, word [hiddenSectors + 2]
	add ax, word [reservedSectors]
	mov [rootDirectorySector], ax

%%.read_next_sector:
	push cx
	push ax
	xor bx, bx
	call read_sector

%%.check_next_entry:
	mov cx, 11					; Filenames are eleven bytes long
	mov di, bx					; Address of directory entry
	mov si, %1
	repz cmpsb					; Compares the filename to memory
	je %%.found_file

	add bx, word BYTES_PER_DIRECTORY_ENTRY ; Entries are 32 bytes, move to the next one
	cmp bx, word [bytesPerSector]
	jl %%.check_next_entry

	pop ax
	inc ax						; Check the next sector next time
	pop cx
	loopnz %%.read_next_sector	; Try find another sector
	jmp boot_failed

%%.found_file:
	; bx is still pointing to the start of the entry
	mov ax, [es:bx + ENTRY_CLUSTER_NUMBER_OFFSET]
%endmacro

	
; void read_file_from_disk(cluster, load_into)
%macro read_file_from_disk 2
	; Sets the segment that will receive the file
	mov ax, %2
	mov es, ax
	xor bx, bx					; Memory offset of loading = 0

	; The start of the FAT
	mov cx, %1

%%.read_next_sector:
	; Locates the sector
	; Sector to read = current FAT entry + root directory - 2
	mov ax, cx
	add ax, [rootDirectorySector]
	add ax, [rootDirectorySize]
	sub ax, 2

	; Reads the sector
	push cx
	call read_sector
	pop cx
	add bx, [bytesPerSector]	; Moves pointer to the next sector

	; Gets the next sector from the FAT
	push ds
	mov dx, FAT_LOAD_SEGMENT
	mov ds, dx

	; Source index is the current FAT entry times 4
	mov si, cx					
	add si, cx
	add si, cx
	add si, cx

	; Reads the entry from memory
	mov ecx, [ds:si]

	; Check for the end-of-chain mark
	and ecx, 0x0fffffff
	pop ds
	cmp ecx, 0x0ffffff8
	
	jl %%.read_next_sector
%endmacro

	
; void try_set_a20_bios()
%macro try_set_a20_bios 0
	mov ax, 0x2401
	int 0x15
%endmacro

; void try_set_a20_keyboard()
%macro try_set_a20_keyboard 0
	; Disables interrupts
	cli

	; Disables the keyboard
	call wait_for_8042_command
	mov al, 0xad
	out 0x64, al

	; Asks for input
	call wait_for_8042_command
	mov al, 0xd0
	out 0x64, al

	; Reads the input
	call wait_for_8042_data
	in al, 0x60
	push eax

	; Writes to the output
	call wait_for_8042_command
	mov al, 0xd1
	out 0x64, al
	
	; Write input back, with bit 2 set
	call wait_for_8042_command
	pop eax
	or al, 2
	out 0x60, al

	; Re-enables the keyboard
	call wait_for_8042_command
	mov al, 0xae
	out 0x64, al

	call wait_for_8042_command
	sti
%endmacro

; void try_set_a20_fast()
%macro try_set_a20_fast 0
	; Reads, sets bit 2, writes
	in al, 0x92
	or al, 2
	out 0x92, al
%endmacro
	
; void is_a20_done()
%macro is_a20_done 0
	call test_a20
	cmp ax, 0
	jne .done
%endmacro
	
; void try_enable_a20()
%macro try_enable_a20 0
try_enable:
	is_a20_done
	try_set_a20_bios
	is_a20_done
	try_set_a20_keyboard
	is_a20_done
	try_set_a20_fast
	is_a20_done

.fail:
	print a20FailedMessage
	call reboot

.done:
	print a20SuccessMessage
%endmacro
	
%endif
