//  ===== Date Created: 13 September, 2020 ===== 

#include "io.hpp"

u8 port_in_8(u16 port)
{
	u8 data;
	asm volatile("inb %1, %0" : "=a"(data) : "dN"(port));
	return data;
}
	
u16 port_in_16(u16 port)
{
	u16 data;
	asm volatile("inw %1, %0" : "=a"(data) : "dN"(port));
	return data;
}
	
u32 port_in_32(u16 port)
{
	u32 data;
	asm volatile("inl %1, %0" : "=a"(data) : "dN"(port));
	return data;
}
	
void port_out_8(u16 port, u8 data)
{
	asm volatile("outb %0, %1" : : "a"(data), "dN"(port));	
}

void port_out_16(u16 port, u16 data)
{
	asm volatile("outw %0, %1" : : "a"(data), "dN"(port));	
}

void port_out_32(u16 port, u32 data)
{
	asm volatile("outl %0, %1" : : "a"(data), "dN"(port));	
}
