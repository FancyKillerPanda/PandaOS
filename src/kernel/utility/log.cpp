//  ===== Date Created: 13 September, 2020 ===== 

#include "log.hpp"
#include "io.hpp"
#include "display.hpp"

// A toggle for outputting to the host stdout
bool shouldUseSerialPort = true;

// Whether we are actually using the host stdout for logging
bool serialPortLogging = false;

#define COM1 ((u16) 0x3f8)
#define COM2 ((u16) 0x2f8)
#define COM3 ((u16) 0x3e8)
#define COM4 ((u16) 0x2e8)

#define SERIAL_PORT COM1

void serial_port_init()
{
	constexpr u16 interruptEnableRegister = SERIAL_PORT + 1;
	constexpr u16 fifoControlRegister = SERIAL_PORT + 2;
	constexpr u16 lineControlRegister = SERIAL_PORT + 3;
	constexpr u16 modemControlRegister = SERIAL_PORT + 4;

	// Works when Divisor Latch Access Bit (DLAB) is set
	// This is the divisor for the Baud rate
	constexpr u16 divisorLowRegister = SERIAL_PORT + 0;
	constexpr u16 divisorHighRegister = SERIAL_PORT + 1;

	// Disables all interrupts temporarily
	port_out_8(interruptEnableRegister, 0x00);

	// Enables DLAB
	port_out_8(lineControlRegister, 1 << 7);

	// Sets the DLAB divisor to 3
	port_out_8(divisorLowRegister, 0x03);
	port_out_8(divisorHighRegister, 0x00);

	// Eight data bits, no parity, one stop bit
	port_out_8(lineControlRegister, 0x03);

	// Enables FIFO, clears the queues, sets a 14-byte IRQ threshold
	port_out_8(fifoControlRegister, 0xc7);

	// Enables IRQs, sets RTS/DSR
	port_out_8(modemControlRegister, 0x0b);
}

void log_init()
{
	if (shouldUseSerialPort)
	{
		serial_port_init();
		serialPortLogging = true;
		log_info("Using serial port for logging.");
	}
}

bool is_serial_ready()
{
	constexpr u16 lineStatusRegister = SERIAL_PORT + 5;
	return port_in_8(lineStatusRegister) & 0x20;
}

void log_serial(const u8* str1, const u8* str2)
{
	if (!serialPortLogging)
	{
		return;
	}

	while (*str1)
	{
		while (!is_serial_ready());
		
		port_out_8(SERIAL_PORT, *str1);
		str1 += 1;
	}

	while (*str2)
	{
		while (!is_serial_ready());
		
		port_out_8(SERIAL_PORT, *str2);
		str2 += 1;
	}

	while (!is_serial_ready());
	port_out_8(SERIAL_PORT, '\n');
}

void do_prefix_formatting(const u8*& message)
{
	while (*message == '\t' || *message == '\n' || *message == '\b')
	{
		print_char(*message);
		message += 1;
	}
}

void log_plain(const u8* message)
{
	print(message);
	print("\n");
	log_serial(message, "");
}

void log_info_plain(const u8* message)
{
	do_prefix_formatting(message);
	
	print("Info: ", 0x09);
	print(message);
	print("\n");

	log_serial("Info: ", message);
}
	
void log_warning_plain(const u8* message)
{
	do_prefix_formatting(message);
	
	print("Warning: ", 0x0e);
	print(message);
	print("\n");

	log_serial("Warning: ", message);
}
	
void log_error_plain(const u8* message)
{
	print("\n");
	do_prefix_formatting(message);
	
	print("Error: ", 0x04);
	print(message);
	print("\n");	

	log_serial("Error: ", message);
}

void log(const u8* message, ...)
{
	va_list argsPointer;
	va_start(argsPointer, message);

	// TODO(fkp): snprintf so we can output serial properly too
	vprintf(message, argsPointer);
	print("\n");

	va_end(argsPointer);
}

void log_info(const u8* message, ...)
{
	va_list argsPointer;
	va_start(argsPointer, message);
	
	do_prefix_formatting(message);
	
	print("Info: ", 0x09);
	vprintf(message, argsPointer);
	print("\n");

	va_end(argsPointer);
	
	// TODO(fkp): snprintf so we can output serial properly too
}

void log_warning(const u8* message, ...)
{
	va_list argsPointer;
	va_start(argsPointer, message);
	
	do_prefix_formatting(message);
	
	print("Warning: ", 0x0e);
	vprintf(message, argsPointer);
	print("\n");

	va_end(argsPointer);
	
	// TODO(fkp): snprintf so we can output serial properly too
}

void log_error(const u8* message, ...)
{
	va_list argsPointer;
	va_start(argsPointer, message);
	
	print("\n");
	do_prefix_formatting(message);
	
	print("Error: ", 0x04);
	vprintf(message, argsPointer);
	print("\n");

	va_end(argsPointer);
	
	// TODO(fkp): snprintf so we can output serial properly too
}
