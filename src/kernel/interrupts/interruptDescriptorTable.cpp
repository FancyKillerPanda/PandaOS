//  ===== Date Created: 24 September, 2020 ===== 

#include "system.hpp"
#include "io.hpp"
#include "interruptDescriptorTable.hpp"
#include "handleInterrupts.hpp"
#include "handleExceptions.hpp"

struct PACKED_STRUCT InterruptDescriptorTableEntry
{
	u16 offsetLow = 0;
	u16 selector = 0;
	u8 zero = 0;
	u8 type = 0;
	u16 offsetHigh = 0;
};

static_assert(sizeof(InterruptDescriptorTableEntry) == 8, "InterruptDescriptorTableEntry size is incorrect.");

InterruptDescriptorTableEntry interruptDescriptorTableEntries[256];

void init_interrupt_descriptor_table()
{
	// Remaps the PIC
	constexpr u8 masterPICCommandAddress = 0x20;
	constexpr u8 masterPICDataAddress = 0x21;
	constexpr u8 slavePICCommandAddress = 0xa0;
	constexpr u8 slavePICDataAddress = 0xa1;

	constexpr u8 initSequenceStart = 0x11;
	constexpr u8 masterPICOffset = 0x20;
	constexpr u8 slavePICOffset = 0x28;
	constexpr u8 mode8086 = 0x01;
	
	// NOTE(fkp): The waits make sure this doesn't happen too quickly
	port_out_8(masterPICCommandAddress, initSequenceStart);
	port_wait();
	port_out_8(slavePICCommandAddress, initSequenceStart);
	port_wait();
	port_out_8(masterPICDataAddress, masterPICOffset);
	port_wait();
	port_out_8(slavePICDataAddress, slavePICOffset);
	port_wait();
	port_out_8(masterPICDataAddress, 0x04); // Tells master there's a slave on IRQ2
	port_wait();
	port_out_8(slavePICDataAddress, 0x02); // Tells slave its cascade identity
	port_wait();
	port_out_8(masterPICDataAddress, mode8086);
	port_wait();
	port_out_8(slavePICDataAddress, mode8086);
	port_wait();
	port_out_8(masterPICDataAddress, 0x00);
	port_wait();
	port_out_8(slavePICDataAddress, 0x00);
	port_wait();

	/* TODO(fkp): Inline intialisation of this needs memcpy and stuff
	const u32 interruptRequestAddresses[16] = {
		(u32) handle_interrupt_request0, (u32) handle_interrupt_request1,
		(u32) handle_interrupt_request2, (u32) handle_interrupt_request3,
		(u32) handle_interrupt_request4, (u32) handle_interrupt_request5,
		(u32) handle_interrupt_request6, (u32) handle_interrupt_request7,
		(u32) handle_interrupt_request8, (u32) handle_interrupt_request9,
		(u32) handle_interrupt_request10, (u32) handle_interrupt_request11,
		(u32) handle_interrupt_request12, (u32) handle_interrupt_request13,
		(u32) handle_interrupt_request14, (u32) handle_interrupt_request15,
	};
	*/

	u32 interruptRequestAddresses[48];
	interruptRequestAddresses[0] = (u32) handle_division_by_zero_exception;
	interruptRequestAddresses[1] = (u32) handle_debug_exception;
	interruptRequestAddresses[2] = (u32) handle_non_maskable_exception;
	interruptRequestAddresses[3] = (u32) handle_breakpoint_exception;
	interruptRequestAddresses[4] = (u32) handle_overflow_exception;
	interruptRequestAddresses[5] = (u32) handle_bound_range_exceeded_exception;
	interruptRequestAddresses[6] = (u32) handle_invalid_opcode_exception;
	interruptRequestAddresses[7] = (u32) handle_device_not_available_exception;
	interruptRequestAddresses[8] = (u32) handle_double_fault_exception;
	interruptRequestAddresses[10] = (u32) handle_invalid_tss_exception;
	interruptRequestAddresses[11] = (u32) handle_segment_not_present_exception;
	interruptRequestAddresses[12] = (u32) handle_stack_segment_fault_exception;
	interruptRequestAddresses[13] = (u32) handle_general_protection_fault_exception;
	interruptRequestAddresses[14] = (u32) handle_page_fault_exception;
	interruptRequestAddresses[16] = (u32) handle_x87_floating_point_exception;
	interruptRequestAddresses[17] = (u32) handle_alignment_check_exception;
	interruptRequestAddresses[18] = (u32) handle_machine_check_exception;
	interruptRequestAddresses[19] = (u32) handle_simd_floating_point_exception;
	interruptRequestAddresses[20] = (u32) handle_virtualisation_exception;
	interruptRequestAddresses[30] = (u32) handle_security_exception;
	
	interruptRequestAddresses[32] = (u32) handle_interrupt_request0;
	interruptRequestAddresses[33] = (u32) handle_interrupt_request1;
	interruptRequestAddresses[34] = (u32) handle_interrupt_request2;
	interruptRequestAddresses[35] = (u32) handle_interrupt_request3;
	interruptRequestAddresses[36] = (u32) handle_interrupt_request4;
	interruptRequestAddresses[37] = (u32) handle_interrupt_request5;
	interruptRequestAddresses[38] = (u32) handle_interrupt_request6;
	interruptRequestAddresses[39] = (u32) handle_interrupt_request7;
	interruptRequestAddresses[40] = (u32) handle_interrupt_request8;
	interruptRequestAddresses[41] = (u32) handle_interrupt_request9;
	interruptRequestAddresses[42] = (u32) handle_interrupt_request10;
	interruptRequestAddresses[43] = (u32) handle_interrupt_request11;
	interruptRequestAddresses[44] = (u32) handle_interrupt_request12;
	interruptRequestAddresses[45] = (u32) handle_interrupt_request13;
	interruptRequestAddresses[46] = (u32) handle_interrupt_request14;
	interruptRequestAddresses[47] = (u32) handle_interrupt_request15;

	const u32 interruptDescriptorTableAddress = (u32) interruptDescriptorTableEntries;
	u32 interruptDescriptorTablePointer[2];
	constexpr u16 kernelCodeSegmentOffset = 0x08;
	constexpr u8 interruptGate = 0x8e;
	constexpr u8 firstInterrupt = 32;

#define SET_UP_HANDLER(index, offset)									\
	InterruptDescriptorTableEntry& entry##offset = interruptDescriptorTableEntries[index]; \
	entry##offset.offsetLow = interruptRequestAddresses[index] & 0x0000ffff; \
	entry##offset.selector = kernelCodeSegmentOffset;					\
	entry##offset.zero = 0;												\
	entry##offset.type = interruptGate;							\
	entry##offset.offsetHigh = (interruptRequestAddresses[index] & 0xffff0000) >> 16;
	
	// Exceptions
	SET_UP_HANDLER(0, a);
	SET_UP_HANDLER(1, b);
	SET_UP_HANDLER(2, c);
	SET_UP_HANDLER(3, d);
	SET_UP_HANDLER(4, e);
	SET_UP_HANDLER(5, f);
	SET_UP_HANDLER(6, g);
	SET_UP_HANDLER(7, h);
	SET_UP_HANDLER(7, i);
	SET_UP_HANDLER(8, j);
	SET_UP_HANDLER(10, k);
	SET_UP_HANDLER(11, l);
	SET_UP_HANDLER(12, m);
	SET_UP_HANDLER(13, n);
	SET_UP_HANDLER(14, o);
	SET_UP_HANDLER(16, p);
	SET_UP_HANDLER(17, q);
	SET_UP_HANDLER(18, r);
	SET_UP_HANDLER(19, s);
	SET_UP_HANDLER(20, t);
	SET_UP_HANDLER(30, u);
	
	// IRQs
	SET_UP_HANDLER(32, 0);
	SET_UP_HANDLER(33, 1);
	SET_UP_HANDLER(34, 2);
	SET_UP_HANDLER(35, 3);
	SET_UP_HANDLER(36, 4);
	SET_UP_HANDLER(37, 5);
	SET_UP_HANDLER(38, 6);
	SET_UP_HANDLER(39, 7);
	SET_UP_HANDLER(40, 8);
	SET_UP_HANDLER(41, 9);
	SET_UP_HANDLER(42, 10);
	SET_UP_HANDLER(43, 11);
	SET_UP_HANDLER(44, 12);
	SET_UP_HANDLER(45, 13);
	SET_UP_HANDLER(46, 14);
	SET_UP_HANDLER(47, 15);

#undef SET_UP_HANDLER

	// Fills in the IDT descriptor
	u32 tableTotalSize = sizeof(InterruptDescriptorTableEntry) * 256;
	u32 tableOffset = ((interruptDescriptorTableAddress & 0xffff) << 16);
	interruptDescriptorTablePointer[0] = tableTotalSize + tableOffset;
	interruptDescriptorTablePointer[1] = interruptDescriptorTableAddress >> 16;

	asm volatile("lidt %0" : : "m"(interruptDescriptorTablePointer));
	asm volatile("sti" ::);
}
