//  ===== Date Created: 24 September, 2020 ===== 

#include "handleInterrupts.hpp"
#include "log.hpp"
#include "io.hpp"

#define SEND_END_OF_INTERRUPT_SIGNAL() port_out_8(0x20, 0x20)
#define SEND_END_OF_INTERRUPT_SIGNAL_EXT() port_out_8(0xa0, 0x20); port_out_8(0x20, 0x20);

// System timer
INTERRUPT_FUNCTION void handleInterruptRequest0(InterruptFrame* frame)
{
	log_info("System timer interrupt (0).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Keyboard
INTERRUPT_FUNCTION void handleInterruptRequest1(InterruptFrame* frame)
{
	log_info("Keyboard interrupt (1).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// EGA/VGA/bus mouse/network adaptors
INTERRUPT_FUNCTION void handleInterruptRequest2(InterruptFrame* frame)
{
	log_info("Adaptors interrupt (2).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Second serial port
INTERRUPT_FUNCTION void handleInterruptRequest3(InterruptFrame* frame)
{
	log_info("Second serial port interrupt (3).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// First serial port
INTERRUPT_FUNCTION void handleInterruptRequest4(InterruptFrame* frame)
{
	log_info("First serial port interrupt (4).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Hard disk drive
INTERRUPT_FUNCTION void handleInterruptRequest5(InterruptFrame* frame)
{
	log_info("Hard disk drive interrupt (5).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Floppy disk drive
INTERRUPT_FUNCTION void handleInterruptRequest6(InterruptFrame* frame)
{
	log_info("Floppy disk drive interrupt (6).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Parallel printer port
INTERRUPT_FUNCTION void handleInterruptRequest7(InterruptFrame* frame)
{
	log_info("Parallel printer port interrupt (7).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Motherboard real-time clock
INTERRUPT_FUNCTION void handleInterruptRequest8(InterruptFrame* frame)
{
	log_info("Motherboard real-time clock interrupt (8).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Works with IRQ2?
INTERRUPT_FUNCTION void handleInterruptRequest9(InterruptFrame* frame)
{
	log_info("Interrupt request 9.");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Likely available
INTERRUPT_FUNCTION void handleInterruptRequest10(InterruptFrame* frame)
{
	log_info("Interrupt request 10.");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Likely available
INTERRUPT_FUNCTION void handleInterruptRequest11(InterruptFrame* frame)
{
	log_info("Interrupt request 11.");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Mouse / likely available
INTERRUPT_FUNCTION void handleInterruptRequest12(InterruptFrame* frame)
{
	log_info("Interrupt request 12.");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Numeric coprocessor
INTERRUPT_FUNCTION void handleInterruptRequest13(InterruptFrame* frame)
{
	log_info("Numeric coprocessor interrupt (13).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Hard disk drive
INTERRUPT_FUNCTION void handleInterruptRequest14(InterruptFrame* frame)
{
	log_info("Hard disk drive interrupt (14).");
	SEND_END_OF_INTERRUPT_SIGNAL();
}

// Likely available
INTERRUPT_FUNCTION void handleInterruptRequest15(InterruptFrame* frame)
{
	log_info("Interrupt request 15.");
	SEND_END_OF_INTERRUPT_SIGNAL();
}
