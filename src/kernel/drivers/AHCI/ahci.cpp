//  ===== Date Created: 11 August, 2021 ===== 

#include "drivers/AHCI/ahci.hpp"
#include "drivers/PCI/pci.hpp"

struct HBAPort
{
	u32 commandListBaseAddress;
	u32 commandListBaseAddressUpper;
	u32 fisBaseAddress;
	u32 fisBaseAddressUpper;
	u32 interruptStatus;
	u32 interruptEnable;
	u32 commandAndStatus;
	
	u32 reserved0;
	
	u32 taskFileData;
	u32 signature;
	u32 sataStatus;
	u32 sataControl;
	u32 sataError;
	u32 sataActive;
	u32 commandIssue;
	u32 sataNotification;
	u32 fisSwitchControl;

	u32 reserved1[11];
	u32 vendor[4];
};
static_assert(sizeof(HBAPort) == 128, "HBAPort must be 128 bytes.");

struct HBAMemorySpace
{
	// Generic Host Control - behaviour of the whole controller
	u32 capability;
	u32 globalHostControl;
	u32 interruptStatus;
	u32 portsUsed;
	u32 version;
	u32 cccControl; // Command completion coalescing - control
	u32 cccPorts; // Command completion coalescing - ports
	u32 enclosureManagementLocation;
	u32 enclosureManagementControl;
	u32 capabilityExtended;
	u32 handoffControl; // BIOS-OS handoff control and status

	// Unused by us
	u8 reserved[116];
	u8 vendor[96];

	// At least 1 port, we can potentially access up to 32
	HBAPort ports[1];
};
static_assert(sizeof(HBAMemorySpace) == 384, "HBAMemorySpace must be 384 bytes.");

void init_ahci()
{
	Peripheral ahciController = get_peripheral(0x01, 0x06);

	// Gets the AHCI base memory address
	u32 bar5 = read_config_32(ahciController, BAR_5_OFFSET);
	void* bar5Page = (void*) (bar5 & ~0xfff);
	HBAMemorySpace* hbaMemorySpace = (HBAMemorySpace*) map_to_physical(bar5Page);

	printf("Ports used: %x\n", hbaMemorySpace->portsUsed);
}
