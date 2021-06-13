//  ===== Date Created: 08 December, 2020 ===== 

#include "display/textDisplay.hpp"
#include "memory/operations.hpp"
#include "system/io.hpp"

static u16* const TEXT_VIDEO_MEMORY = (u16*) 0xc00b8000;
constexpr u16 SCREEN_ROWS = 25;
constexpr u16 SCREEN_COLS = 80;

#define ENABLE_HOST_CONSOLE_LOG 1
constexpr u16 HOST_CONSOLE_LOG_PORT = 0xe9;

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

void scroll_screen_up(u8 numberOfLines, u8 attribute)
{
	u16 blank = ' ' | (attribute << 8);

	if (numberOfLines == 0)
	{
		// Scrolls enough to have one line available
		if (cursorRow >= SCREEN_ROWS)
		{
			numberOfLines = cursorRow + 1 - SCREEN_ROWS;
		}
		else
		{
			return;
		}
	}

	memcpy(TEXT_VIDEO_MEMORY,
		   TEXT_VIDEO_MEMORY + (numberOfLines * SCREEN_COLS),
		   SCREEN_ROWS * SCREEN_COLS * 2);
	memset_16(TEXT_VIDEO_MEMORY + (SCREEN_ROWS - numberOfLines) * SCREEN_COLS,
			  blank,
			  SCREEN_COLS * numberOfLines);
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

	if (cursorRow >= SCREEN_ROWS)
	{
		scroll_screen_up();
	}

#if ENABLE_HOST_CONSOLE_LOG
	port_out_8(HOST_CONSOLE_LOG_PORT, character);
#endif
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

void print_integer(u32 integer, u8 minimumWidth = 0, u8 attribute = 0x07)
{
	// NOTE(fkp): 32-bit integer has 10 digits maximum in decimal
	u8 digits[10];
	u8 currentIndex = STACK_ARRAY_LENGTH(digits) - 1;
	memset(digits, 0, STACK_ARRAY_LENGTH(digits));

	while (integer / 10)
	{
		digits[currentIndex] = '0' + (integer % 10);
		integer /= 10;
		currentIndex -= 1;
	}

	digits[currentIndex] = '0' + (integer % 10);
	u8 numberOfDigits = STACK_ARRAY_LENGTH(digits) - currentIndex;

	for (s16 i = 0; i < (s16) minimumWidth - (s16) numberOfDigits; i++)
	{
		print_char(' ', attribute);
	}
	
	for (u8 i = currentIndex; i < STACK_ARRAY_LENGTH(digits); i++)
	{
		print_char(digits[i], attribute);
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

void print_hex_integer(u32 integer, u8 minimumWidth = 0, u8 attribute = 0x07)
{
	// NOTE(fkp): minimumWidth includes the "0x", but we don't use that
	if (minimumWidth >= 2)
	{
		minimumWidth -= 2;
	}
	else
	{
		minimumWidth = 0;
	}
	
	// NOTE(fkp): 32-bit integer has 8 digits maximum in hex (plus
	// space for "0x").  We don't actually put anything into the first
	// two indicies, but we need them for the currentIndex
	// calculations.
	u8 digits[10];
	u8 currentIndex = STACK_ARRAY_LENGTH(digits) - 1;
	memset(digits, 0, STACK_ARRAY_LENGTH(digits));
	
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

		if (currentIndex == 0)
		{
			break;
		}

		currentIndex -= 1;
	}

	currentIndex += 1;
	print_char('0', attribute);
	print_char('x', attribute);

	u8 numberOfDigits = STACK_ARRAY_LENGTH(digits) - currentIndex;

	for (s16 i = 0; i < (s16) minimumWidth - (s16) numberOfDigits; i++)
	{
		print_char('0', attribute);
	}

	for (u8 i = currentIndex; i < STACK_ARRAY_LENGTH(digits); i++)
	{
		print_char(digits[i], attribute);
	}
}

void vprintf(const u8* string, va_list argsPointer)
{
	shouldMoveCursor = false;
	bool isConsumingFormatter = false;
	u32 minimumWidth = 0;
	
	while (*string)
	{
		if (*string == '%')
		{
			if (isConsumingFormatter)
			{
				print_char('%');
				isConsumingFormatter = false;
			}
			else
			{
				isConsumingFormatter = true;
				minimumWidth = 0;
			}
		}
		else
		{
			if (isConsumingFormatter)
			{
				switch (*string)
				{
				case 'c':
				{
					print_char((u8) va_arg(argsPointer, u32));
					isConsumingFormatter = false;
				} break;
				
				case 'd':
				{
					print_integer(va_arg(argsPointer, u32), minimumWidth);
					isConsumingFormatter = false;
				} break;
				
				case 's':
				{
					print(va_arg(argsPointer, u8*));
					isConsumingFormatter = false;
				} break;
				
				case 'x':
				{
					print_hex_integer(va_arg(argsPointer, u32), minimumWidth);
					isConsumingFormatter = false;
				} break;

				default:
				{
					if (*string >= '0' && *string <= '9')
					{
						minimumWidth *= 10;
						minimumWidth += *string - '0';
					}
					else
					{
						print_char(*string);
						isConsumingFormatter = false;
					}
				} break;
				}
			}
			else
			{
				print_char(*string);
			}
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
