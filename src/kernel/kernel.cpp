/*  ===== Date Created: 11 September, 2020 =====  */

#include "display/textDisplay.hpp"
#include "display/graphicalDisplay.hpp"

#include "utility/log.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "display/videoMode.hpp"

#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"
#include "memory/heapAllocator.hpp"
#include "memory/memoryMap.hpp"

// TODO(fkp): Fix this. Something about the VMM makes
// it not work when the pointer is passed somewhere.
/*
u8 testBitmap[] = {
	0b00000000, 0b00000000,
	0b01111110, 0b00010000,
	0b01111110, 0b00111000,
	0b01111110, 0b01111100,
	0b00000000, 0b00000000,
};
*/

extern "C" void start_kernel(MemoryMap* memoryMap, VideoMode* videoMode)
{
	init_interrupt_descriptor_table();

	// clear_screen();
	// move_cursor(14, 0); // This is because the bootloader logged some text
	move_cursor(16, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();
	print("\n");

	
	read_memory_map(memoryMap);
	init_physical_allocator(memoryMap);
	init_virtual_allocator();
	init_heap_allocator();

	init_video(videoMode);
	// draw_rect(100, 100, 200, 100, 0xff0000);
	// draw_rect(400, 100, 200, 100, 0x00ff00);
	// draw_rect(150, 300, 300, 200, 0x0000ff);

	/*
	u8* testBitmap = (u8*) malloc(10 * sizeof(u8));
	testBitmap[0] = 0b00000000;
	testBitmap[1] = 0b00000000;
	testBitmap[2] = 0b01111110;
	testBitmap[3] = 0b00010000;
	testBitmap[4] = 0b01111110;
	testBitmap[5] = 0b00111000;
	testBitmap[6] = 0b01111110;
	testBitmap[7] = 0b01111100;
	testBitmap[8] = 0b00000000;
	testBitmap[9] = 0b00000000;
	
	draw_bitmap(testBitmap, 0, 0, 16, 5, 0xffffff);
	*/

	// 8infy's mouse cursor...
	u8* testBitmap = (u8*) malloc(38 * sizeof(u8));
	testBitmap[0] = 0b00000001; testBitmap[1] = 0b00000000;
	testBitmap[2] = 0b00000011; testBitmap[3] = 0b00000000;
	testBitmap[4] = 0b00000111; testBitmap[5] = 0b00000000;
	testBitmap[6] = 0b00001111; testBitmap[7] = 0b00000000;
	testBitmap[8] = 0b00011111; testBitmap[9] = 0b00000000;
	testBitmap[10] = 0b00111111; testBitmap[11] = 0b00000000;
	testBitmap[12] = 0b01111111; testBitmap[13] = 0b00000000;
	testBitmap[14] = 0b11111111; testBitmap[15] = 0b00000000;
	testBitmap[16] = 0b11111111; testBitmap[17] = 0b00000001;
	testBitmap[18] = 0b11111111; testBitmap[19] = 0b00000011;
	testBitmap[20] = 0b11111111; testBitmap[21] = 0b00000111;
	testBitmap[22] = 0b11111111; testBitmap[23] = 0b00001111;
	testBitmap[24] = 0b01111111; testBitmap[25] = 0b00000000;
	testBitmap[26] = 0b11110111; testBitmap[27] = 0b00000000;
	testBitmap[28] = 0b11110011; testBitmap[29] = 0b00000000;
	testBitmap[30] = 0b11100001; testBitmap[31] = 0b00000001;
	testBitmap[32] = 0b11100000; testBitmap[33] = 0b00000001;
	testBitmap[34] = 0b11000000; testBitmap[35] = 0b00000000;
	testBitmap[36] = 0b00000000; testBitmap[37] = 0b0000000;
	
	draw_bitmap(testBitmap, 0, 0, 16, 19, 0xffffff);
	draw_bitmap(testBitmap, 16, 0, 16, 19, 0xff0000);
	draw_bitmap(testBitmap, 32, 0, 16, 19, 0x00ff00);
	draw_bitmap(testBitmap, 48, 0, 16, 19, 0x0000ff);
	draw_bitmap(testBitmap, 0, 32, 16, 19, 0x888888);
	draw_bitmap(testBitmap, 16, 32, 16, 19, 0xffff00);
	draw_bitmap(testBitmap, 32, 32, 16, 19, 0x00ffff);
	draw_bitmap(testBitmap, 48, 32, 16, 19, 0xff00ff);

	free(testBitmap);
	
	log_info("\nFinished, now hanging...");
	while (true);
}
