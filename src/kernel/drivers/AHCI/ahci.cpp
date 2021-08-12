//  ===== Date Created: 11 August, 2021 ===== 

#include "drivers/AHCI/ahci.hpp"
#include "drivers/AHCI/structures/frameInformationStructure.hpp"
#include "drivers/AHCI/structures/hostBusAdaptor.hpp"

#include "drivers/PCI/pci.hpp"

void init_ahci()
{
	Peripheral ahciController = get_peripheral(0x01, 0x06);

	// Gets the AHCI base memory address
	u32 bar5 = read_config_32(ahciController, BAR_5_OFFSET);
	void* bar5Page = (void*) (bar5 & ~0xfff);
	HBAMemorySpace* hbaMemorySpace = (HBAMemorySpace*) map_to_physical(bar5Page);

	printf("Ports used: %x\n", hbaMemorySpace->portsUsed);
}
