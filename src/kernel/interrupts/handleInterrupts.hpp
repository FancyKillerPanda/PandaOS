//  ===== Date Created: 15 December, 2020 ===== 

#if !defined(HANDLEINTERRUPTS_HPP)
#define HANDLEINTERRUPTS_HPP

#include "system/common.hpp"

struct PACKED_STRUCT InterruptFrame
{
	u16 instructionPointer = 0;
	u16 codeSegment = 0;
	u16 flags = 0;
	u16 stackPointer = 0;
	u16 stackSegment = 0;
};

static_assert(sizeof(InterruptFrame) == 10, "InterruptFrame size must be 10 bytes.");

void handle_interrupt_request_0(InterruptFrame* frame); // System timer
void handle_interrupt_request_1(InterruptFrame* frame); // Keyboard
void handle_interrupt_request_2(InterruptFrame* frame); // EGA/VGA/bus mouse/network adaptors
void handle_interrupt_request_3(InterruptFrame* frame); // Second serial port
void handle_interrupt_request_4(InterruptFrame* frame); // First serial port
void handle_interrupt_request_5(InterruptFrame* frame); // Hard disk drive
void handle_interrupt_request_6(InterruptFrame* frame); // Floppy disk drive
void handle_interrupt_request_7(InterruptFrame* frame); // Parallel printer port
void handle_interrupt_request_8(InterruptFrame* frame); // Motherboard real-time clock
void handle_interrupt_request_9(InterruptFrame* frame); // Works with IRQ2?
void handle_interrupt_request_10(InterruptFrame* frame); // Likely available
void handle_interrupt_request_11(InterruptFrame* frame); // Likely available
void handle_interrupt_request_12(InterruptFrame* frame); // Mouse / likely available
void handle_interrupt_request_13(InterruptFrame* frame); // Numeric coprocessor
void handle_interrupt_request_14(InterruptFrame* frame); // Hard disk drive
void handle_interrupt_request_15(InterruptFrame* frame); // Likely available

#endif
