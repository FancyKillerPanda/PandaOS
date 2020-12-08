//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(IO_HPP)
#define IO_HPP

#include "system/common.hpp"

u8 port_in_8(u16 port);
u16 port_in_16(u16 port);
u32 port_in_32(u16 port);

void port_out_8(u16 port, u8 data);
void port_out_16(u16 port, u16 data);
void port_out_32(u16 port, u32 data);

void port_wait();

#endif
