/*  ===== Date Created: 12 September, 2020 =====  */

#include "display.hpp"
#include "system.hpp"
#include "io.hpp"
#include "memory/operations.hpp"

#define VIDEO_MEMORY ((u16*) 0xb8000)
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

// TODO(fkp): I don't like how this logging stuff is split between
// display and log.
#define CONSOLE_LOG_PORT ((u16) 0xe9)
#define CONSOLE_LOGGING 1

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

#if CONSOLE_LOGGING
	port_out_8(CONSOLE_LOG_PORT, character);
#endif
}

void print_integer(u32 integer, u8 attribute)
{
	// NOTE(fkp): 32bit integer has 10 digits maximum in decimal
	u8 digits[10];
	u8 currentIndex = STACK_ARRAY_LENGTH(digits) - 1;
	set_memory(digits, STACK_ARRAY_LENGTH(digits), 0);

	while (integer / 10)
	{
		digits[currentIndex] = '0' + (integer % 10);
		integer /= 10;
		currentIndex -= 1;
	}

	digits[currentIndex] = '0' + (integer % 10);

	for (u8 digit : digits)
	{
		if (digit)
		{
			print_char(digit, attribute);
		}
	}
}

u8 format_hex_digit(u8 digit)
{
	if (digit % 16 <= 9)
	{
		return '0' + (digit % 16);
	}
	else
	{
		return 'a' + ((digit % 16) - 10);
	}
}

void print_hex_integer(u32 integer, u8 attribute)
{
	// NOTE(fkp): 32bit integer has 8 digits maximum in hex (plus 2 for '0x')
	u8 digits[10];
	u8 currentIndex = STACK_ARRAY_LENGTH(digits) - 1;
	set_memory(digits, STACK_ARRAY_LENGTH(digits), 0);
	
	while (integer / 16)
	{
		digits[currentIndex] = format_hex_digit(integer % 16);
		integer /= 16;
		currentIndex -= 1;
	}

	digits[currentIndex] = format_hex_digit(integer % 16);
	currentIndex -= 1;

	while (STACK_ARRAY_LENGTH(digits) - 1 - currentIndex != 2 &&
		   STACK_ARRAY_LENGTH(digits) - 1 - currentIndex != 4 &&
		   STACK_ARRAY_LENGTH(digits) - 1 - currentIndex != 8)
	{
		digits[currentIndex] = '0';
		currentIndex -= 1;
	}
	
	digits[currentIndex] = 'x';
	currentIndex -= 1;
	digits[currentIndex] = '0';
	
	for (u8 digit : digits)
	{
		if (digit)
		{
			print_char(digit, attribute);
		}
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

void vprintf(const u8* string, va_list argsPointer)
{
	shouldMoveCursor = false;
	
	while (*string)
	{
		switch (*string)
		{
		case '%':
		{
			string += 1;

			switch (*string)
			{
			case 'd':
			{
				print_integer(va_arg(argsPointer, u32));
			} break;

			case 'x':
			{
				print_hex_integer(va_arg(argsPointer, u32));
			} break;
				
			case 's':
			{
				print(va_arg(argsPointer, u8*));
			} break;

			case 'c':
			{
				print_char((u8) va_arg(argsPointer, u32));
			} break;
			
			default:
			{
				print_char(*string);
			} break;
			}
		} break;

		default:
		{
			print_char(*string);
		} break;
		}

		string += 1;
	}

	shouldMoveCursor = true;
	move_cursor(cursorRow, cursorCol);
}

void printf(const u8* string, ...)
{
	va_list argsPointer;
	va_start(argsPointer, string);

	vprintf(string, argsPointer);
	
	va_end(argsPointer);
}
