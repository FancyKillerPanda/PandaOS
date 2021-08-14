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
	
	// TODO(fkp): Mark as uncacheable
//	HBAMemorySpace& hbaMemorySpace = *(HBAMemorySpace*) map_to_physical(bar5Page, 2);
//	HBACommandHeader** portCommandHeaderList = (HBACommandHeader**) map_to_physical();
	HBAMemorySpace& hbaMemorySpace = *(HBAMemorySpace*) allocate_virtual_range(2 * PAGE_SIZE, nullptr, bar5Page);

	// Iterates through the implemented ports
	for (u8 i = 0; i < 32; i++)
	{
		if (!(hbaMemorySpace.portsImplemented & (1 << i)))
		{
			// The port is not implemented
			continue;
		}

		// TODO(fkp): Stop command
		
		HBAPort& port = hbaMemorySpace.ports[i];
		printf("Port: %x\n", port.commandListBaseAddress);

		// TODO(fkp): Start command
	}
}
