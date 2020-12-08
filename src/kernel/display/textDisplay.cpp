//  ===== Date Created: 08 December, 2020 ===== 

#include "display/textDisplay.hpp"
#include "system/io.hpp"

static u16* const TEXT_VIDEO_MEMORY = (u16*) 0xb8000;
constexpr u16 SCREEN_ROWS = 25;
constexpr u16 SCREEN_COLS = 80;

static u8 cursorRow = 0;
static u8 cursorCol = 0;

// Whether the hardware cursor should be moved.
// This is turned off at the start of printing a long string.
static bool shouldMoveCursor = true;

void move_cursor(u8 row, u8 col)
{
	cursorCol = col;
	cursorRow = row;

	if (shouldMoveCursor)
	{
		u16 location = (cursorRow * SCREEN_COLS) + cursorCol;

		// The low byte
		port_out_8(0x3d4, 0x0f);
		port_out_8(0x3d5, (u8) (location & 0xff));

		// The high byte
		port_out_8(0x3d4, 0x0e);
		port_out_8(0x3d5, (u8) ((location >> 8) & 0xff));
	}
}

void print_char(u8 character, u8 attribute)
{
	u16 location = (cursorRow * SCREEN_COLS) + cursorCol;

	switch (character)
	{
	case '\b':
	{
		if (cursorCol > 0)
		{
			u16 blank = ' ' | (attribute << 8);
			*(TEXT_VIDEO_MEMORY + location - 1) = blank;
			move_cursor(cursorRow, cursorCol - 1);
		}
	} break;
		
	case '\r':
	{
		move_cursor(cursorRow, 0);
	} break;
	
	case '\n':
	{
		move_cursor(cursorRow + 1, 0);
	} break;

	case '\t':
	{
		// Cursor moves to a location divisible by four
		move_cursor(cursorRow, (cursorCol + 4) & ~(4 - 1));
	} break;
	
	default:
	{
		// Printable characters are space and above
		if (character >= ' ')
		{
			u16 data = character | (attribute << 8);
			*(TEXT_VIDEO_MEMORY + location) = data;
			move_cursor(cursorRow, cursorCol + 1);
		}
	} break;
	}
	
	if (cursorCol >= SCREEN_COLS)
	{
		move_cursor(cursorRow + 1, 0);
	}
}

void print(const u8* string, u8 attribute)
{
	shouldMoveCursor = false;

	while (*string)
	{
		print_char(*string, attribute);
		string += 1;
	}

	shouldMoveCursor = true;
	move_cursor(cursorRow, cursorCol);
}
