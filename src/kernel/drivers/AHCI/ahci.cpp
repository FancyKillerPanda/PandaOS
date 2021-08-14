//  ===== Date Created: 11 August, 2021 ===== 

#include "drivers/AHCI/ahci.hpp"
#include "drivers/AHCI/structures/frameInformationStructure.hpp"
#include "drivers/AHCI/structures/hostBusAdaptor.hpp"

#include "drivers/PCI/pci.hpp"

void start_port_command_engine(HBAPort& port)
{
	// Waits until command list stops running
	while (port.commandAndStatus.commandListRunning);

	// Enables FIS and command list processing
	port.commandAndStatus.fisReceiveEnable = 1;
	port.commandAndStatus.start = 1;
}

void stop_port_command_engine(HBAPort& port)
{
	// Disables FIS and command list processing
	port.commandAndStatus.start = 0;
	port.commandAndStatus.fisReceiveEnable = 0;

	// Waits until command list and FIS stop running
	while (port.commandAndStatus.commandListRunning ||
		   port.commandAndStatus.fisReceiveRunning);
}

void init_ahci()
{
	Peripheral ahciController = get_peripheral(0x01, 0x06);

	// Gets the AHCI base memory address
	u32 bar5 = read_config_32(ahciController, BAR_5_OFFSET);
	void* bar5Page = (void*) (bar5 & ~0xfff);
	
	// TODO(fkp): Mark as uncacheable
	HBAMemorySpace& hbaMemorySpace = *(HBAMemorySpace*) allocate_virtual_range(2 * PAGE_SIZE, nullptr, bar5Page);

	// Allocates space for every port's command lists.
	// NOTE(fkp): We use the maximum possible value for the number of
	// slots per port because the command list must be 1 kiB aligned.
	constexpr u32 portCommandListMaxSlots = 32;
	constexpr u32 portCommandListSize = portCommandListMaxSlots * sizeof(HBACommandHeader);
	u32 numberOfPorts = hbaMemorySpace.capability.numberOfPorts + 1;
	u32 commandListArraySize = numberOfPorts * portCommandListSize;
	u32 portCommandListSlots = hbaMemorySpace.capability.numberOfCommandSlots + 1;

	using CommandList = HBACommandHeader[portCommandListMaxSlots];
	CommandList* commandListArray = (CommandList*) allocate_virtual_range(commandListArraySize);
	log_info("Allocating %x bytes (~%d pages) for command lists (starts %x).",
			 commandListArraySize, (commandListArraySize / PAGE_SIZE) + 1,
			 (u32) commandListArray);

	// Allocates space for every port's command tables. The number of
	// command tables is equal to the number of implemented ports
	// multiplied by the number of command slots per port.
	// NOTE(fkp): We allocate a page for each command table
	// to give room for however many PRDTs we want.
	u32 commandTableSize = PAGE_SIZE;
	u32 commandTableListSize = portCommandListSlots * commandTableSize; // Per port
	u32 commandTableListArraySize = numberOfPorts * commandTableListSize;
	
	using CommandTableList = HBACommandTable*;
	CommandTableList* commandTableListArray = (CommandTableList*) allocate_virtual_range(commandTableListArraySize);
	log_info("Allocating %x bytes (%d pages) for command tables (starts %x).",
			 commandTableListArraySize, commandTableListArraySize / PAGE_SIZE,
			 (u32) commandTableListArray);
	
	// Allocates space for every port's Received FIS.
	u32 fisArraySize = sizeof(HBAFIS) * numberOfPorts;
	HBAFIS* fisArray = (HBAFIS*) allocate_virtual_range(fisArraySize);
	log_info("Allocating %x bytes (~%d pages) for FIS array (starts %x).",
			 fisArraySize, (fisArraySize / PAGE_SIZE) + 1, (u32) fisArray);

	// Iterates through the implemented ports
	for (u8 i = 0; i < 32; i++)
	{
		if (!(hbaMemorySpace.portsImplemented & (1 << i)))
		{
			// The port is not implemented
			continue;
		}

		HBAPort& port = hbaMemorySpace.ports[i];
		stop_port_command_engine(port);

		// Rebases the current port's command list
		port.commandListBaseAddress = (u32) get_mapping(&commandListArray[i]);
		port.commandListBaseAddressUpper = 0;
		memset(&commandListArray[i], 0, portCommandListSize);

		// Rebases the current port's command tables
		HBACommandHeader* commandList = commandListArray[i];
		HBACommandTable* commandTableList = (HBACommandTable*) ((u32) commandTableListArray + (i * commandTableListSize));

		for (u8 slot = 0; slot < portCommandListSlots; slot++)
		{
			u32 commandTableBaseAddress = (u32) commandTableList + (slot * commandTableSize);
			commandList[slot].commandTableBaseAddress = (u32) get_mapping((void*) commandTableBaseAddress);
			commandList[slot].commandTableBaseAddressUpper = 0;
			memset((void*) commandTableBaseAddress, 0, commandTableSize);
		}

		// Rebases the current port's FIS
		port.fisBaseAddress = (u32) get_mapping(&fisArray[i]);
		port.fisBaseAddressUpper = 0;
		memset(&fisArray[i], 0, fisArraySize);
		
		start_port_command_engine(port);
	}

	log_info("Initialised AHCI controller.");
}
