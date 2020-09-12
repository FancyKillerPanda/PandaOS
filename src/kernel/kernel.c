/*  ===== Date Created: 11 September, 2020 =====  */

#define true 1
#define false 0
typedef unsigned char u8;
typedef unsigned short u16;

void test_print_string()
{
	u8* address = (u8*) 0xb8000;
	const char* string = "Hello, world!";
	u16 stringSize = 13;

	for (u16 i = 0; i < stringSize; i++)
	{
		*address = (u8) string[i];
		address += 1;
		*address = (u8) 0x9f;
		address += 1;
	}
}

void start_kernel()
{
	test_print_string();
	while (true);
}
