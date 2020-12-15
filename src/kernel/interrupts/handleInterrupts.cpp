//  ===== Date Created: 15 December, 2020 ===== 

#include "interrupts/handleInterrupts.hpp"
#include "system/io.hpp"
#include "utility/log.hpp"

#define SEND_END_OF_INTERRUPT_SIGNAL() port_out_8(0x20, 0x20)
#define SEND_END_OF_INTERRUPT_SIGNAL_EXT() port_out_8(0xa0, 0x20); port_out_8(0x20, 0x20)

// Returns true if the interrupt was spurious
bool handle_potential_spurious_interrupt(bool extended)
{
	constexpr u8 MASTER_PIC_COMMAND = 0x20;
	constexpr u8 SLAVE_PIC_COMMAND = 0xa0;

	// Reads the In-Service Register
	port_out_8(MASTER_PIC_COMMAND, 0x0b);
	port_out_8(SLAVE_PIC_COMMAND, 0x0b);
	u16 inServiceRegister = (port_in_8(SLAVE_PIC_COMMAND) << 8) | port_in_8(MASTER_PIC_COMMAND);

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
INTERRUPT_FUNCTION void handle_interrupt_request_0(InterruptFrame* frame)
{
	UNUSED(frame);

	// log_info("System timer interrupt (0).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Keyboard
INTERRUPT_FUNCTION void handle_interrupt_request_1(InterruptFrame* frame)
{
	UNUSED(frame);
	
	log_info("Keyboard interrupt (1).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// EGA/VGA/bus mouse/network adaptors
INTERRUPT_FUNCTION void handle_interrupt_request_2(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Adaptors interrupt (2).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Second serial port
INTERRUPT_FUNCTION void handle_interrupt_request_3(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Second serial port interrupt (3).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// First serial port
INTERRUPT_FUNCTION void handle_interrupt_request_4(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("First serial port interrupt (4).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Hard disk drive
INTERRUPT_FUNCTION void handle_interrupt_request_5(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Hard disk drive interrupt (5).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Floppy disk drive
INTERRUPT_FUNCTION void handle_interrupt_request_6(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Floppy disk drive interrupt (6).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Parallel printer port
INTERRUPT_FUNCTION void handle_interrupt_request_7(InterruptFrame* frame)
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
INTERRUPT_FUNCTION void handle_interrupt_request_8(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Motherboard real-time clock interrupt (8).");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Works with IRQ2?
INTERRUPT_FUNCTION void handle_interrupt_request_9(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 9.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Likely available
INTERRUPT_FUNCTION void handle_interrupt_request_10(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 10.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Likely available
INTERRUPT_FUNCTION void handle_interrupt_request_11(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 11.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Mouse / likely available
INTERRUPT_FUNCTION void handle_interrupt_request_12(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Interrupt request 12.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Numeric coprocessor
INTERRUPT_FUNCTION void handle_interrupt_request_13(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Numeric coprocessor interrupt (13).");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Hard disk drive
INTERRUPT_FUNCTION void handle_interrupt_request_14(InterruptFrame* frame)
{
	UNUSED(frame);

	log_info("Hard disk drive interrupt (14).");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}

// Likely available
INTERRUPT_FUNCTION void handle_interrupt_request_15(InterruptFrame* frame)
{
	UNUSED(frame);

	if (handle_potential_spurious_interrupt(true))
	{
		return;
	}
	
	log_info("Interrupt request 15.");
	SEND_END_OF_INTERRUPT_SIGNAL_EXT();
}
