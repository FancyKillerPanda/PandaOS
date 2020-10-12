//  ===== Date Created: 13 September, 2020 ===== 

#if !defined(PORT_HPP)
#define PORT_HPP

#include "system.hpp"

u8 port_in_8(u16 port);
u16 port_in_16(u16 port);
u32 port_in_32(u16 port);

void port_out_8(u16 port, u8 data);
void port_out_16(u16 port, u16 data);
void port_out_32(u16 port, u32 data);

void port_wait();

#endif
