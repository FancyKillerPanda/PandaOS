//  ===== Date Created: 11 August, 2021 ===== 

#include "drivers/AHCI/ahci.hpp"
#include "drivers/PCI/pci.hpp"

void init_ahci()
{
	Peripheral ahciController = get_peripheral(0x01, 0x06);
}
