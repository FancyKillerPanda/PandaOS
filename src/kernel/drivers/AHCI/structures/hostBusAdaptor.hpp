//  ===== Date Created: 12 August, 2021 ===== 

#if !defined(HOSTBUSADAPTOR_HPP)
#define HOSTBUSADAPTOR_HPP

#include "system/common.hpp"

struct HBAPort
{
	u32 commandListBaseAddress;
	u32 commandListBaseAddressUpper;
	u32 fisBaseAddress;
	u32 fisBaseAddressUpper;
	u32 interruptStatus;
	u32 interruptEnable;
	
	struct
	{
		u32 start : 1;
		u32 unused0 : 3;
		u32 fisReceiveEnable : 1;
		u32 unused1 : 9;
		u32 fisReceiveRunning : 1;
		u32 commandListRunning : 1;
		u32 unused2 : 16;
	} commandAndStatus;
	
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

struct HBACapability
{
	u32 numberOfPorts : 5; // 0 means 1 port is supported
	u32 supportsExternalSATA : 1;
	u32 supportsEnclosureManagement : 1;
	u32 supportsCommandCompletionCoalescing : 1;
	u32 numberOfCommandSlots : 5; // 0 means 1 slot per port supported
	u32 partialStateCapable : 1;
	u32 slumberStateCapable : 1;
	u32 pioMultipleDRQBlock : 1;
	u32 supportsFISBasedSwitching : 1;
	u32 supportsPortMultiplier : 1;
	u32 supportsAHCIModeOnly : 1;
	u32 interfaceSpeedSupport : 4;
	u32 supportsCommandListOverride : 1;
	u32 supportsActivityLED : 1;
	u32 supportsAggressiveLinkPowerManagement : 1;
	u32 supportsStaggeredSpinUp : 1;
	u32 supportsMechanicalPresenceSwitch : 1;
	u32 supportsSNotificationRegister : 1;
	u32 supportsNativeCommandQueuing : 1;
	u32 supports64bitAddressing : 1;
};
static_assert(sizeof(HBACapability) == 4, "HBACapability must be 4 bytes.");

struct HBAMemorySpace
{
	// Generic Host Control - behaviour of the whole controller
	HBACapability capability;
	u32 globalHostControl;
	u32 interruptStatus;
	u32 portsImplemented;
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
	HBAPort ports[32];
};
static_assert(sizeof(HBAMemorySpace) == 4352, "HBAMemorySpace must be 4352 bytes.");

struct HBAFIS
{
	DMASetupFIS dmaSetupFIS;
	u32 padding0;

	PIOSetupFIS pioSetupFIS;
	u32 padding1[3];

	DeviceToHostFIS deviceToHostFIS;
	u32 padding2;

	SetDeviceBitsFIS setDeviceBitsFIS;

	u8 unknownFIS[64];
	u8 reserved[96];
};
static_assert(sizeof(HBAFIS) == 256, "HBAFIS must be 256 bytes.");

struct HBACommandHeader
{
	u8 commandFISLength : 5; // Measured in DWORDs
	u8 atapi : 1;
	u8 direction : 1; // 0 = device to host, 1 = host to device
	u8 prefetchable : 1;

	u8 reset : 1;
	u8 bist : 1;
	u8 clearBusyFlag : 1;
	u8 reserved0 : 1;
	u8 portMultiplier : 4;

	u16 physicalRegionDescriptorTableLength; // Number of entries
	u32 physicalRegionDescriptorByteCount;
	u32 commandTableBaseAddress;
	u32 commandTableBaseAddressUpper;

	u32 reserved1[4];
};
static_assert(sizeof(HBACommandHeader) == 32, "HBACommandHeader must be 32 bytes.");

struct HBAPhysicalRegionDescriptorTableEntry
{
	u32 dataBaseAddress;
	u32 dataBaseAddressUpper;
	u32 reserved0;

	u32 dataByteCount : 22; // 4 MiB max
	u32 reserved1 : 9;
	u32 interruptOnCompletion : 1;
};
static_assert(sizeof(HBAPhysicalRegionDescriptorTableEntry) == 16,
			  "HBAPhysicalRegionDescriptorTableEntry must be 16 bytes.");

struct HBACommandTable
{
	u8 commandFIS[64];
	u8 atapiCommand[16]; // Either 12 or 16 bytes
	u8 reserved[48];

	HBAPhysicalRegionDescriptorTableEntry entries[];
};
static_assert(sizeof(HBACommandTable) == 128, "HBACommandTable must be 128 bytes.");

#endif
