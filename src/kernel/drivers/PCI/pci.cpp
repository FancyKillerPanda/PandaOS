//  ===== Date Created: 10 August, 2021 ===== 

#include "system/common.hpp"
#include "system/io.hpp"

#include "utility/log.hpp"

bool pciInitialised = false;

constexpr u16 CONFIG_ADDRESS = 0xcf8;
constexpr u16 CONFIG_DATA = 0xcfc;

constexpr u16 NUMBER_OF_BUSSES = 255; // Should be 256, but bus will be a u8
constexpr u16 NUMBER_OF_DEVICES_PER_BUS = 32;
constexpr u16 NUMBER_OF_FUNCTIONS_PER_DEVICE = 8;

constexpr u8 MULTI_FUNCTION_DEVICE_BIT = 0x80;

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

void find_all_devices()
{
	ASSERT(!pciInitialised, "Attempting to initialise PCI driver twice.");
	log_info("\n===== PCI Peripherals =====");
	
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

				// Checks the function details
				u8 baseClass = read_config_8(bus, device, function, CLASS_OFFSET);
				u8 subClass = read_config_8(bus, device, function, SUB_CLASS_OFFSET);
				u8 headerType = read_config_8(bus, device, function, HEADER_TYPE_OFFSET);

				peripherals.push({
					bus,
					device,
					function,
					headerType,
					baseClass,
					subClass,
				});

				printf("Bus: %d\t Device: %d\t Function: %d\t Class %x (%x)\n",
					   bus, device, function, baseClass, subClass);

				// For the first function, check if this is a
				// multifunction device. If it is not, break out of
				// the loop.
				if (function == 0)
				{
					if (!(headerType & MULTI_FUNCTION_DEVICE_BIT))
					{
						break;
					}
				}
			}
		}
	}

	log_info("===========================");
	pciInitialised = true;
}

Peripheral get_peripheral(u16 baseClass, u16 subClass)
{
	ASSERT(pciInitialised, "Cannot get PCI peripheral before initialisation.");
	
	for (usize i = 0; i < peripherals.size; i++)
	{
		if (peripherals[i].baseClass == baseClass &&
			peripherals[i].subClass == subClass)
		{
			return peripherals[i];
		}
	}

	log_error("Could not find peripheral with class %x (%x).", baseClass, subClass);
	while (true);
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

u8 read_config_8(const Peripheral& peripheral, u8 offset)
{
	return read_config_8(peripheral.bus, peripheral.device, peripheral.function, offset);
}

u16 read_config_16(const Peripheral& peripheral, u8 offset)
{
	return read_config_16(peripheral.bus, peripheral.device, peripheral.function, offset);
}

u32 read_config_32(const Peripheral& peripheral, u8 offset)
{
	return read_config_32(peripheral.bus, peripheral.device, peripheral.function, offset);
}
