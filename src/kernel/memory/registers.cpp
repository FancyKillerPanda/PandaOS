//  ===== Date Created: 17 December, 2020 ===== 

#include "memory/registers.hpp"
#include "utility/log.hpp"

void print_registers(const RegisterState& rs)
{
	log("===== Registers =====\n"
		"eax: %x\t\tebx: %x\t\tecx: %x\t\tedx: %x\n"
		"esp: %x\t\tebp: %x\t\tesi: %x\t\tedi: %x\n\n"
		"cs: %x\t\tds: %x\t\tes: %x\n"
		"fs: %x\t\tgs: %x\t\tss: %x\n"
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
