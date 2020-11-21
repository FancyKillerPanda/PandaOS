//  ===== Date Created: 24 September, 2020 ===== 

#include "handleInterrupts.hpp"
#include "io.hpp"
#include "display.hpp"
#include "utility/log.hpp"
#include "input/keyboard.hpp"

#define SEND_END_OF_INTERRUPT_SIGNAL() port_out_8(0x20, 0x20)
#define SEND_END_OF_INTERRUPT_SIGNAL_EXT() port_out_8(0xa0, 0x20); port_out_8(0x20, 0x20)

// Returns true if the interrupt was spurious
bool handle_potential_spurious_interrupt(bool extended)
{
	constexpr u8 masterPICCommandAddress = 0x20;
	constexpr u8 slavePICCommandAddress = 0xa0;

	// Reads the In-Service Register
	port_out_8(masterPICCommandAddress, 0x0b);
	port_out_8(slavePICCommandAddress, 0x0b);
	u16 inServiceRegister = (port_in_8(slavePICCommandAddress) << 8) | port_in_8(masterPICCommandAddress);

	if (inServiceRegister & 0x0080 ||
		(extended && inServiceRegister & 0x8000))
	{
		// This interrupt is not spurious
		return false;
	}

	// This interrupt is spurious
	if (extended)
	{
		SEND_END_OF_INTERRUPT_SIGNAL();
	}
	
	log_warning("Spurious interrupt detected.");
	return true;
}

// System timer
INTERRUPT_FUNCTION void handle_interrupt_request0(InterruptFrame* frame)
{
	UNUSED(frame);
	// log_info("System timer interrupt (0).");
	
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Keyboard
INTERRUPT_FUNCTION void handle_interrupt_request1(InterruptFrame* frame)
{
	UNUSED(frame);
	// log_info("Keyboard interrupt (1).");

	u8 scancode = port_in_8(0x60);
	u8 status = port_in_8(0x61);
	port_out_8(0x61, status | 0x80); // Toggles the high bit, signals that we want more
	
	Key::Code keycode;
		
	if (keysPressed[Key::LeftShift] || keysPressed[Key::RightShift])
	{
		keycode = convert_scancode_to_keycode(scancode, true, KeyboardRegion::EnglishUS);
	}
	else
	{
		keycode = convert_scancode_to_keycode(scancode, false, KeyboardRegion::EnglishUS);
	}
	
	keysPressed[keycode] = !(scancode & 0x80);

	// TODO(fkp): Get rid of this when we have a proper event system
	if (keysPressed[keycode])
	{
		print_char(convert_keycode_to_character(keycode));
	}
	
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// EGA/VGA/bus mouse/network adaptors
INTERRUPT_FUNCTION void handle_interrupt_request2(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Adaptors interrupt (2).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Second serial port
INTERRUPT_FUNCTION void handle_interrupt_request3(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Second serial port interrupt (3).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// First serial port
INTERRUPT_FUNCTION void handle_interrupt_request4(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("First serial port interrupt (4).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Hard disk drive
INTERRUPT_FUNCTION void handle_interrupt_request5(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Hard disk drive interrupt (5).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Floppy disk drive
INTERRUPT_FUNCTION void handle_interrupt_request6(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Floppy disk drive interrupt (6).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Parallel printer port
INTERRUPT_FUNCTION void handle_interrupt_request7(InterruptFrame* frame)
{
	UNUSED(frame);

	if (handle_potential_spurious_interrupt(false))
	{
		return;
	}
	
	log_info("Parallel printer port interrupt (7).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Motherboard real-time clock
INTERRUPT_FUNCTION void handle_interrupt_request8(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Motherboard real-time clock interrupt (8).");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Works with IRQ2?
INTERRUPT_FUNCTION void handle_interrupt_request9(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 9.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Likely available
INTERRUPT_FUNCTION void handle_interrupt_request10(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 10.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Likely available
INTERRUPT_FUNCTION void handle_interrupt_request11(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 11.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Mouse / likely available
INTERRUPT_FUNCTION void handle_interrupt_request12(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 12.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Numeric coprocessor
INTERRUPT_FUNCTION void handle_interrupt_request13(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Numeric coprocessor interrupt (13).");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Hard disk drive
INTERRUPT_FUNCTION void handle_interrupt_request14(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Hard disk drive interrupt (14).");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Likely available
INTERRUPT_FUNCTION void handle_interrupt_request15(InterruptFrame* frame)
{
	UNUSED(frame);

	if (handle_potential_spurious_interrupt(true))
	{
		return;
	}
	
	log_info("Interrupt request 15.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}
