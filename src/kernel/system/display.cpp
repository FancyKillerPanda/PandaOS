/*  ===== Date Created: 12 September, 2020 =====  */

#include "system.hpp"
#include "display.hpp"

#define VIDEO_MEMORY ((u16*) 0xb8000)
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

// Current cursor position
u8 cursorX = 0;
u8 cursorY = 0;

void print_char(u8 character, u8 attribute)
{
	u16 location = (cursorY * SCREEN_COLS) + cursorX;
	u16 data = character | (attribute << 8);
	*(VIDEO_MEMORY + location) = data;

	cursorX += 1;

	if (cursorX >= SCREEN_COLS)
	{
		cursorX = 0;
		cursorY += 1;
	}

	if (cursorY >= SCREEN_ROWS)
	{
		scroll_screen();
	}
}

void print(const u8* string, u8 attribute)
{
	while (*string)
	{
		print_char(*string, attribute);
		string += 1;
	}
}
