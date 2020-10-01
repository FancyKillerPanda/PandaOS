//  ===== Date Created: 26 September, 2020 =====

#include "handleExceptions.hpp"
#include "log.hpp"

void dump_registers(InterruptFrame* frame)
{
	log("===== Registers =====\n"
		"CS:IP - %x:%x\n"
		"SS:SP - %x:%x\n"
		"Flags - %x\n"
		"=====================",
		frame->codeSegment, frame->instructionPointer,
		frame->stackSegment, frame->stackPointer,
		frame->flags);
}

INTERRUPT_FUNCTION void handle_division_by_zero_exception(InterruptFrame* frame)
{
	log_error("Division by zero exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_debug_exception(InterruptFrame* frame)
{
	log_error("Debug exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_non_maskable_exception(InterruptFrame* frame)
{
	log_error("Non-maskable exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_breakpoint_exception(InterruptFrame* frame)
{
	log_error("Breakpoint hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_overflow_exception(InterruptFrame* frame)
{
	log_error("Overflow exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_bound_range_exceeded_exception(InterruptFrame* frame)
{
	log_error("Bounds exceeded exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_invalid_opcode_exception(InterruptFrame* frame)
{
	log_error("Invalid opcode exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_device_not_available_exception(InterruptFrame* frame)
{
	log_error("Device not available exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_double_fault_exception(InterruptFrame* frame)
{
	log_error("Double fault exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_invalid_tss_exception(InterruptFrame* frame)
{
	log_error("Invalid TSS exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_segment_not_present_exception(InterruptFrame* frame)
{
	log_error("Segment not present exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_stack_segment_fault_exception(InterruptFrame* frame)
{
	log_error("Stack-segment fault exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_general_protection_fault_exception(InterruptFrame* frame)
{
	log_error("General protection fault exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_page_fault_exception(InterruptFrame* frame)
{
	log_error("Page fault exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_x87_floating_point_exception(InterruptFrame* frame)
{
	log_error("x87 floating point exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_alignment_check_exception(InterruptFrame* frame)
{
	log_error("Alignment check exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_machine_check_exception(InterruptFrame* frame)
{
	log_error("Machine check exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_simd_floating_point_exception(InterruptFrame* frame)
{
	log_error("SIMD floating point exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_virtualisation_exception(InterruptFrame* frame)
{
	log_error("Virtualisation exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_security_exception(InterruptFrame* frame)
{
	log_error("Security exception hit!");
	dump_registers(frame);

	while (true);
}

INTERRUPT_FUNCTION void handle_triple_fault_exception(InterruptFrame* frame)
{
	log_error("Triple fault exception hit!");
	dump_registers(frame);

	while (true);
}
