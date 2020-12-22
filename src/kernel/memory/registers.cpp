//  ===== Date Created: 17 December, 2020 ===== 

#include "memory/registers.hpp"
#include "utility/log.hpp"

void print_registers(const RegisterState& rs)
{
	log("===== Registers =====\n"
		"eax: %10x\t\tebx: %10x\t\tecx: %10x\t\tedx: %10x\n"
		"esp: %10x\t\tebp: %10x\t\tesi: %10x\t\tedi: %10x\n\n"
		"cs: %6x\t\tds: %6x\t\tes: %6x\n"
		"fs: %6x\t\tgs: %6x\t\tss: %6x\n"
		"=====================",
		rs.eax, rs.ebx, rs.ecx, rs.edx,
		rs.esp, rs.ebp, rs.esi, rs.edi,
		rs.cs, rs.ds, rs.es,
		rs.fs, rs.gs, rs.ss);
}

void print_current_registers()
{
	RegisterState registerState;
	get_registers(&registerState);
	print_registers(registerState);
}
