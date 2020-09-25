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
void handleInterruptRequest0(InterruptFrame* frame);

// Keyboard
void handleInterruptRequest1(InterruptFrame* frame);

// EGA/VGA/bus mouse/network adaptors
void handleInterruptRequest2(InterruptFrame* frame);

// Second serial port
void handleInterruptRequest3(InterruptFrame* frame);

// First serial port
void handleInterruptRequest4(InterruptFrame* frame);

// Hard disk drive
void handleInterruptRequest5(InterruptFrame* frame);

// Floppy disk drive
void handleInterruptRequest6(InterruptFrame* frame);

// Parallel printer port
void handleInterruptRequest7(InterruptFrame* frame);

// Motherboard real-time clock
void handleInterruptRequest8(InterruptFrame* frame);

// Works with IRQ2?
void handleInterruptRequest9(InterruptFrame* frame);

// Likely available
void handleInterruptRequest10(InterruptFrame* frame);

// Likely available
void handleInterruptRequest11(InterruptFrame* frame);

// Mouse / likely available
void handleInterruptRequest12(InterruptFrame* frame);

// Numeric coprocessor
void handleInterruptRequest13(InterruptFrame* frame);

// Hard disk drive
void handleInterruptRequest14(InterruptFrame* frame);

// Likely available
void handleInterruptRequest15(InterruptFrame* frame);

#endif
