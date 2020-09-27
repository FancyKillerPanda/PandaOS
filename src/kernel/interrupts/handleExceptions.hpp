//  ===== Date Created: 26 September, 2020 ===== 

#if !defined(HANDLEEXCEPTIONS_HPP)
#define HANDLEEXCEPTIONS_HPP

#include "handleInterrupts.hpp"

void handle_division_by_zero_exception(InterruptFrame* frame);
void handle_debug_exception(InterruptFrame* frame);
void handle_non_maskable_exception(InterruptFrame* frame);
void handle_breakpoint_exception(InterruptFrame* frame);
void handle_overflow_exception(InterruptFrame* frame);
void handle_bound_range_exceeded_exception(InterruptFrame* frame);
void handle_invalid_opcode_exception(InterruptFrame* frame);
void handle_device_not_available_exception(InterruptFrame* frame);
void handle_double_fault_exception(InterruptFrame* frame);
void handle_invalid_tss_exception(InterruptFrame* frame);
void handle_segment_not_present_exception(InterruptFrame* frame);
void handle_stack_segment_fault_exception(InterruptFrame* frame);
void handle_general_protection_fault_exception(InterruptFrame* frame);
void handle_page_fault_exception(InterruptFrame* frame);
void handle_x87_floating_point_exception(InterruptFrame* frame);
void handle_alignment_check_exception(InterruptFrame* frame);
void handle_machine_check_exception(InterruptFrame* frame);
void handle_simd_floating_point_exception(InterruptFrame* frame);
void handle_virtualisation_exception(InterruptFrame* frame);
void handle_security_exception(InterruptFrame* frame);
void handle_triple_fault_exception(InterruptFrame* frame);

#endif
