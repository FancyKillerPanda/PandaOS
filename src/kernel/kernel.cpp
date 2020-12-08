//  ===== Date Created: 02 December, 2020 ===== 

#include "system/common.hpp"

void test_print_string()
{
	u8* address = (u8*) 0xb8000;
	const u8* string = "Hello, world!";
	u16 stringSize = 13;

	for (u16 i = 0; i < stringSize; i++)
	{
		*address = (u8) string[i];
		address += 1;
		*address = (u8) 0x9f;
		address += 1;
	}
}

extern "C" void kmain()
{
	test_print_string();
	while (true);
}
