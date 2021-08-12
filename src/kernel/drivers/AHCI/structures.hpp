//  ===== Date Created: 12 August, 2021 ===== 

#if !defined(STRUCTURES_HPP)
#define STRUCTURES_HPP

#include "system/common.hpp"

enum class FISType : u8
{
	HostToDevice = 0x27,
	DeviceToHost = 0x34,
	DMAActivate = 0x39,
	DMASetup = 0x41,
	Data = 0x46,
	BISTActivate = 0x58,
	PIOSetup = 0x5f,
	SetDeviceBits = 0xa1,
};

/*
struct HostToDeviceFIS
{
	FISType type;

	u8 portMultiplier : 4;
	u8 reserved0 : 3;
	u8 commandOrControl : 1; // 0 = control, 1 = command

	u8 command;
	u8 featureLow;
	u8 lba0;
	u8 lba1;
	u8 lba2;
	u8 device;
	u8 lba3;
	u8 lba4;
	u8 lba5;
	u8 featureHigh;

	u16 count;
	u8 isochronousCommandCompletion;
	u8 control;
	u32 reserved1;
};
static_assert(sizeof(HostToDeviceFIS) == 20,
			  "HostToDeviceFIS must be 20 bytes.");
*/

struct DeviceToHostFIS
{
	FISType type;

	u8 portMultiplier : 4;
	u8 reserved0 : 2;
	u8 interrupt : 1;
	u8 reserved1 : 1;

	u8 status;
	u8 error;

	u8 lba0;
	u8 lba1;
	u8 lba2;
	u8 device;
	u8 lba3;
	u8 lba4;
	u8 lba5;
	u8 reserved2;

	u16 count;
	u16 reserved3;
	u32 reserved4;
};
static_assert(sizeof(DeviceToHostFIS) == 20,
			  "DeviceToHostFIS must be 20 bytes.");

struct DMASetupFIS
{
	FISType type;

	u8 portMultiplier : 4;
	u8 reserved0 : 1;
	u8 direction : 1; // 0 = host -> device, 1 = device -> host
	u8 interrupt : 1;
	u8 autoActivate : 1;

	u16 reserved1;
	u64 dmaBufferId;
	u32 reserved2;
	u32 dmaBufferOffset;
	u32 transferCount;
	u32 reserved3;
};
static_assert(sizeof(DMASetupFIS) == 28, "DMASetupFIS must be 28 bytes.");

/*
struct DataFIS
{
	FISType type;

	u8 portMultiplier : 4;
	u8 reserved0 : 4;
	
	u16 reserved1;
	u32 data[1]; // Variable size
};
static_assert(sizeof(DataFIS) == 8, "DataFIS must be 8 bytes.");
*/

struct PIOSetupFIS
{
	FISType type;

	u8 portMultiplier : 4;
	u8 reserved0 : 1;
	u8 direction : 1; // 0 = host -> device, 1 = device -> host
	u8 interrupt : 1;
	u8 reserved1 : 1;

	u8 status;
	u8 error;

	u8 lba0;
	u8 lba1;
	u8 lba2;
	u8 device;
	u8 lba3;
	u8 lba4;
	u8 lba5;
	u8 reserved2;

	u16 count;
	u8 reserved3;
	u8 newStatus;
	u16 transferCount;
	u16 reserved4;
};
static_assert(sizeof(PIOSetupFIS) == 20, "PIOSetupFIS must be 20 bytes.");

struct SetDeviceBitsFIS
{
	FISType type;

	u8 portMultiplier : 4;
	u8 reserved : 2;
	u8 interrupt : 1;
	u8 notification : 1;

	u8 status;
	u8 error;
	u32 protocolSpecific;
};
static_assert(sizeof(SetDeviceBitsFIS) == 8, "SetDeviceBitsFIS must be 8 bytes");

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
