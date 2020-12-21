//  ===== Date Created: 17 December, 2020 ===== 

#if !defined(REGISTERS_HPP)
#define REGISTERS_HPP

#include "system/common.hpp"

struct PACKED_STRUCT RegisterState
{
	// NOTE(fkp): Can't have default values because this needs to be
	// compatible with the C ABI, because it's being used with an
	// `extern "C"` function.
	u32 eax;
	u32 ebx;
	u32 ecx;
	u32 edx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;

	u16 cs;
	u16 ds;
	u16 es;
	u16 fs;
	u16 gs;
	u16 ss;
};

extern "C" void __attribute__((cdecl)) get_registers(RegisterState* registerState);
void print_registers(const RegisterState& registerState);
void print_current_registers();

#endif
