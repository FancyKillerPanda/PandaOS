//  ===== Date Created: 10 August, 2021 ===== 

#if !defined(PCI_HPP)
#define PCI_HPP

#include "utility/dynamicArray.hpp"

constexpr u8 VENDOR_ID_OFFSET = 0x00;
constexpr u8 SUB_CLASS_OFFSET = 0x0a;
constexpr u8 CLASS_OFFSET = 0x0b;
constexpr u8 HEADER_TYPE_OFFSET = 0x0e;
constexpr u8 BAR_5_OFFSET = 0x24;

struct Peripheral
{
	u8 bus = 0;
	u8 device = 0;
	u8 function = 0;

	u8 headerType = 0;
	u16 baseClass = 0;
	u16 subClass = 0;
};

DynamicArray<Peripheral> peripherals;

void find_all_devices();
Peripheral get_peripheral(u16 baseClass, u16 subClass);

u8 read_config_8(u8 bus, u8 device, u8 function, u8 offset);
u16 read_config_16(u8 bus, u8 device, u8 function, u8 offset);
u32 read_config_32(u8 bus, u8 device, u8 function, u8 offset);

u8 read_config_8(const Peripheral& peripheral, u8 offset);
u16 read_config_16(const Peripheral& peripheral, u8 offset);
u32 read_config_32(const Peripheral& peripheral, u8 offset);

#endif
