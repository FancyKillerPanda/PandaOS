//  ===== Date Created: 12 August, 2021 ===== 

#if !defined(FRAMEINFORMATIONSTRUCTURE_HPP)
#define FRAMEINFORMATIONSTRUCTURE_HPP

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

#endif
