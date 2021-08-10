//  ===== Date Created: 10 August, 2021 ===== 

#if !defined(PCI_HPP)
#define PCI_HPP

#include "utility/dynamicArray.hpp"

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

#endif
