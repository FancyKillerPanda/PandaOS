//  ===== Date Created: 10 August, 2021 ===== 

#include "system/common.hpp"
#include "system/io.hpp"

#include "utility/log.hpp"

constexpr u16 CONFIG_ADDRESS = 0xcf8;
constexpr u16 CONFIG_DATA = 0xcfc;

constexpr u16 NUMBER_OF_BUSSES = 255; // Should be 256, but bus will be a u8
constexpr u16 NUMBER_OF_DEVICES_PER_BUS = 32;
constexpr u16 NUMBER_OF_FUNCTIONS_PER_DEVICE = 8;

constexpr u8 MULTI_FUNCTION_DEVICE_BIT = 0x80;

constexpr u8 VENDOR_ID_OFFSET = 0x00;
constexpr u8 SUB_CLASS_OFFSET = 0x0a;
constexpr u8 CLASS_OFFSET = 0x0b;
constexpr u8 HEADER_TYPE_OFFSET = 0x0e;

struct ConfigAddress
{
	u32 offset : 8;
	u32 function : 3;
	u32 device : 5;
	u32 bus : 8;
	u32 reserved : 7;
	u32 enable: 1;
};
static_assert(sizeof(ConfigAddress) == 4, "ConfigAddress struct must be 4 bytes!");

u32 read_config_32(u8 bus, u8 device, u8 function, u8 offset)
{
	ASSERT((offset & 3) == 0, "Offset must be 4-byte aligned for 32 bit PCI read");
	
	ConfigAddress config {
		offset,
		function,
		device,
		bus,
		0,
		1
	};

	u32 configInteger;
	memcpy(&configInteger, &config, sizeof(ConfigAddress));

	port_out_32(CONFIG_ADDRESS, configInteger);
	return port_in_32(CONFIG_DATA);
}

u16 read_config_16(u8 bus, u8 device, u8 function, u8 offset)
{
	ASSERT((offset & 1) == 0, "Offset must be 2-byte aligned for 16 bit PCI read.");
	u32 data = read_config_32(bus, device, function, offset & 0xfc);

	if ((offset & 3) == 0)
	{
		// We're reading the bottom half
		return (u16) data;
	}
	else
	{
		return (u16) (data >> 16);
	}
}

u8 read_config_8(u8 bus, u8 device, u8 function, u8 offset)
{
	u16 data = read_config_16(bus, device, function, offset & 0xfe);

	if ((offset & 1) == 0)
	{
		// We're reading the bottom half
		return (u8) data;
	}
	else
	{
		return (u8) (data >> 8);
	}
}

void find_all_devices()
{
	for (u8 bus = 0; (u16) bus < NUMBER_OF_BUSSES; bus++)
	{
		for (u8 device = 0; device < NUMBER_OF_DEVICES_PER_BUS; device++)
		{
			// First checks first function (might be only function)
			for (u8 function = 0; function < NUMBER_OF_FUNCTIONS_PER_DEVICE; function++)
			{
				u16 vendorId = read_config_16(bus, device, function, VENDOR_ID_OFFSET);

				// Checks if the function exists
				if (vendorId == 0xffff)
				{
					continue;
				}

				// For the first function, check if this is a
				// multifunction device. If it is not, break out of
				// the loop.
				if (function == 0)
				{
					u8 headerType = read_config_8(bus, device, function, HEADER_TYPE_OFFSET);

					if (!(headerType & MULTI_FUNCTION_DEVICE_BIT))
					{
						break;
					}
				}

				// Checks the function
				u8 baseClass = read_config_8(bus, device, function, CLASS_OFFSET);
				u8 subClass = read_config_8(bus, device, function, SUB_CLASS_OFFSET);

				log_info("PCI (bus: %d, device: %d, func: %d): Class %x (%x)",
						 bus, device, function, baseClass, subClass);
			}
		}
	}
}
