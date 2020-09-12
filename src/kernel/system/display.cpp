/*  ===== Date Created: 12 September, 2020 =====  */

#include "display.hpp"
#include "system.hpp"
#include "io.hpp"
#include "memory.hpp"

#define VIDEO_MEMORY ((u16*) 0xb8000)
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

// Current cursor position
u8 cursorRow = 0;
u8 cursorCol = 0;

// Whether the hardware cursor should be moved
// This is turned off at the start of string printing
bool shouldMoveCursor = true;

void enable_cursor()
{
	// Sets the height of the cursor
	u8 cursorStart = 13;
	u8 cursorEnd = 14;
	
	port_out_8(0x3d4, 0x0a);
	port_out_8(0x3d5, (port_in_8(0x3d5) & 0xc0) | cursorStart);

	port_out_8(0x3d4, 0x0b);
	port_out_8(0x3d5, (port_in_8(0x3d5) & 0xe0) | cursorEnd);
}

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

void disable_cursor()
{
	port_out_8(0x3d4, 0x0a);
	port_out_8(0x3d5, 0x20);
}

void clear_screen(u8 attribute)
{
	// The space character
	u16 blank = 0x20 | (attribute << 8);
	set_memory_16(VIDEO_MEMORY, SCREEN_ROWS * SCREEN_COLS * 2, blank);
	move_cursor(0, 0);
}

void scroll_screen_up(u8 numberOfLines, u8 attribute)
{
	u16 blank = 0x20 | (attribute << 8);

	if (numberOfLines == 0)
	{
		// Scrolls enough to have one line available
		if (cursorRow >= SCREEN_ROWS)
		{
			numberOfLines = cursorRow - SCREEN_ROWS + 1;
		}
		else
		{
			return;
		}
	}

	copy_memory(VIDEO_MEMORY + (numberOfLines * SCREEN_COLS), VIDEO_MEMORY, SCREEN_ROWS * SCREEN_COLS * 2);
	set_memory_16(VIDEO_MEMORY + (SCREEN_ROWS - numberOfLines) * SCREEN_COLS, SCREEN_COLS * numberOfLines, blank);
	move_cursor(cursorRow - numberOfLines, cursorCol);
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
			u16 blank = 0x20 | (attribute << 8);
			*(VIDEO_MEMORY + location - 1) = blank;
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
			*(VIDEO_MEMORY + location) = data;
			move_cursor(cursorRow, cursorCol + 1);
		}
	} break;
	}
	
	if (cursorCol >= SCREEN_COLS)
	{
		move_cursor(cursorRow + 1, 0);
	}

	if (cursorRow >= SCREEN_ROWS)
	{
		scroll_screen_up();
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
