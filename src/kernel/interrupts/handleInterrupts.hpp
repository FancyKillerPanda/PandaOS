//  ===== Date Created: 24 September, 2020 ===== 

#if !defined(HANDLEINTERRUPTS_HPP)
#define HANDLEINTERRUPTS_HPP

#include "system.hpp"

struct PACKED_STRUCT InterruptFrame
{
	u16 instructionPointer = 0;
	u16 codeSegment = 0;
	u16 flags = 0;
	u16 stackPointer = 0;
	u16 stackSegment = 0;
};

static_assert(sizeof(InterruptFrame) == 10, "InterruptFrame size is incorrect.");

// System timer
void handle_interrupt_request0(InterruptFrame* frame);

// Keyboard
void handle_interrupt_request1(InterruptFrame* frame);

// EGA/VGA/bus mouse/network adaptors
void handle_interrupt_request2(InterruptFrame* frame);

// Second serial port
void handle_interrupt_request3(InterruptFrame* frame);

// First serial port
void handle_interrupt_request4(InterruptFrame* frame);

// Hard disk drive
void handle_interrupt_request5(InterruptFrame* frame);

// Floppy disk drive
void handle_interrupt_request6(InterruptFrame* frame);

// Parallel printer port
void handle_interrupt_request7(InterruptFrame* frame);

// Motherboard real-time clock
void handle_interrupt_request8(InterruptFrame* frame);

// Works with IRQ2?
void handle_interrupt_request9(InterruptFrame* frame);

// Likely available
void handle_interrupt_request10(InterruptFrame* frame);

// Likely available
void handle_interrupt_request11(InterruptFrame* frame);

// Mouse / likely available
void handle_interrupt_request12(InterruptFrame* frame);

// Numeric coprocessor
void handle_interrupt_request13(InterruptFrame* frame);

// Hard disk drive
void handle_interrupt_request14(InterruptFrame* frame);

// Likely available
void handle_interrupt_request15(InterruptFrame* frame);

#endif
