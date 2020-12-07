; ===== Date Created: 07 December, 2020 ===== 

%macro enable_protected_mode 0
	%%.setup:
		mov si, enableProtectedModeMessage
		call print_string

		; NOTE(fkp): This requires that we have already
		; described the GDT and IDT tables.
		call load_descriptor_tables

		mov word [bootloaderStackPointer], sp

	%%.enable:
		; Enables protected mode
		mov eax, cr0
		or eax, 1
		mov cr0, eax

		; Also clears the prefetch queue
		jmp gdtCode32Offset:%%.setup_segments
		nop
		nop

	%%.setup_segments:
		bits 32
		; Selects the data descriptor for all segments (except cs)
		mov ax, gdtData32Offset
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax
		mov esp, 0x20000		; Still within usable memory
%endmacro

%macro enable_real_mode 0
	%%.setup:
		cli

		; Also clears the prefetch queue
		jmp gdtCode16Offset:%%.switch_to_16_bit_gdt
		nop
		nop

	%%.switch_to_16_bit_gdt:
		bits 16
		mov eax, gdtData16Offset
		mov ds, eax
		mov es, eax
		mov fs, eax
		mov gs, eax
		mov ss, eax

	%%.disable_protected_mode:
		mov eax, cr0
		and eax, 0x7ffffffe		; Disables protected mode and paging
		mov cr0, eax

		; Also clears the prefetch queue
		jmp 0x00:%%.reset_gdt_selector
		nop
		nop

	%%.reset_gdt_selector:
		xor ax, ax
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax
		mov sp, word [bootloaderStackPointer]

	%%.load_real_mode_idt:
		lidt [idtEntryRealMode]

	%%.cleanup:
		sti
		mov si, enableRealModeMessage
		call print_string
%endmacro
