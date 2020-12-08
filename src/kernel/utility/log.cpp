//  ===== Date Created: 08 December, 2020 ===== 

#include "utility/log.hpp"

void do_prefix_formatting(const u8*& message)
{
	while (*message == '\t' || *message == '\n' || *message == '\b')
	{
		print_char(*message);
		message += 1;
	}
}

void log_internal(const u8* prefix, u8 prefixAttribute, const u8* message, va_list argsPointer)
{
	do_prefix_formatting(message);
	
	print(prefix, prefixAttribute);
	vprintf(message, argsPointer);
	print_char('\n');
}

void log(const u8* message, ...)
{
	va_list argsPointer;

	va_start(argsPointer, message);
	log_internal("", 0x00, message, argsPointer);
	va_end(argsPointer);
}

void log_info(const u8* message, ...)
{
	va_list argsPointer;

	va_start(argsPointer, message);
	log_internal("Info: ", 0x09, message, argsPointer);
	va_end(argsPointer);
}

void log_warning(const u8* message, ...)
{
	va_list argsPointer;

	va_start(argsPointer, message);
	log_internal("Warning: ", 0x0e, message, argsPointer);
	va_end(argsPointer);
}
	
void log_error(const u8* message, ...)
{
	va_list argsPointer;

	va_start(argsPointer, message);
	log_internal("Error: ", 0x04, message, argsPointer);
	va_end(argsPointer);
}
