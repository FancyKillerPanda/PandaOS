; ===== Date Created: 17 December, 2020 ===== 

bits 32

; void get_registers(RegisterState* registerState)
global get_registers
get_registers:
push ebp
mov ebp, esp

mov eax, [esp + 8]

mov dword [eax + 0x00], eax		; TODO(fkp): Fix this, it is output pointer
mov dword [eax + 0x04], ebx
mov dword [eax + 0x08], ecx
mov dword [eax + 0x0c], edx
mov dword [eax + 0x10], esp
mov dword [eax + 0x14], ebp
mov dword [eax + 0x18], esi
mov dword [eax + 0x1c], edi

mov word [eax + 0x20], cs
mov word [eax + 0x22], ds
mov word [eax + 0x24], es
mov word [eax + 0x26], fs
mov word [eax + 0x28], gs
mov word [eax + 0x2a], ss

; TODO(fkp): Reset ebx
mov ebx, cr0
mov dword [eax + 0x2c], ebx
mov ebx, cr2
mov dword [eax + 0x30], ebx
mov ebx, cr3
mov dword [eax + 0x34], ebx
mov ebx, cr4
mov dword [eax + 0x38], ebx

pop ebp

ret
