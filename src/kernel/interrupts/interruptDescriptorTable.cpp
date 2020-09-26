//  ===== Date Created: 24 September, 2020 ===== 

#include "system.hpp"
#include "io.hpp"
#include "interruptDescriptorTable.hpp"
#include "handleInterrupts.hpp"

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
		(u32) handleInterruptRequest0, (u32) handleInterruptRequest1,
		(u32) handleInterruptRequest2, (u32) handleInterruptRequest3,
		(u32) handleInterruptRequest4, (u32) handleInterruptRequest5,
		(u32) handleInterruptRequest6, (u32) handleInterruptRequest7,
		(u32) handleInterruptRequest8, (u32) handleInterruptRequest9,
		(u32) handleInterruptRequest10, (u32) handleInterruptRequest11,
		(u32) handleInterruptRequest12, (u32) handleInterruptRequest13,
		(u32) handleInterruptRequest14, (u32) handleInterruptRequest15,
	};
	*/

	u32 interruptRequestAddresses[16];
	interruptRequestAddresses[0] = (u32) handleInterruptRequest0;
	interruptRequestAddresses[1] = (u32) handleInterruptRequest1;
	interruptRequestAddresses[2] = (u32) handleInterruptRequest2;
	interruptRequestAddresses[3] = (u32) handleInterruptRequest3;
	interruptRequestAddresses[4] = (u32) handleInterruptRequest4;
	interruptRequestAddresses[5] = (u32) handleInterruptRequest5;
	interruptRequestAddresses[6] = (u32) handleInterruptRequest6;
	interruptRequestAddresses[7] = (u32) handleInterruptRequest7;
	interruptRequestAddresses[8] = (u32) handleInterruptRequest8;
	interruptRequestAddresses[9] = (u32) handleInterruptRequest9;
	interruptRequestAddresses[10] = (u32) handleInterruptRequest10;
	interruptRequestAddresses[11] = (u32) handleInterruptRequest11;
	interruptRequestAddresses[12] = (u32) handleInterruptRequest12;
	interruptRequestAddresses[13] = (u32) handleInterruptRequest13;
	interruptRequestAddresses[14] = (u32) handleInterruptRequest14;
	interruptRequestAddresses[15] = (u32) handleInterruptRequest15;

	const u32 interruptDescriptorTableAddress = (u32) interruptDescriptorTableEntries;
	u32 interruptDescriptorTablePointer[2];
	constexpr u16 kernelCodeSegmentOffset = 0x08;
	constexpr u8 interruptGate = 0x8e;
	constexpr u8 firstInterrupt = 32;

#define SET_UP_HANDLER(index)											\
	InterruptDescriptorTableEntry& entry##index = interruptDescriptorTableEntries[firstInterrupt + index]; \
	entry##index.offsetLow = interruptRequestAddresses[index] & 0x0000ffff;	\
	entry##index.selector = kernelCodeSegmentOffset;					\
	entry##index.zero = 0;												\
	entry##index.type = interruptGate;									\
	entry##index.offsetHigh = (interruptRequestAddresses[index] & 0xffff0000) >> 16;

	SET_UP_HANDLER(0);
	SET_UP_HANDLER(1);
	SET_UP_HANDLER(2);
	SET_UP_HANDLER(3);
	SET_UP_HANDLER(4);
	SET_UP_HANDLER(5);
	SET_UP_HANDLER(6);
	SET_UP_HANDLER(7);
	SET_UP_HANDLER(8);
	SET_UP_HANDLER(9);
	SET_UP_HANDLER(10);
	SET_UP_HANDLER(11);
	SET_UP_HANDLER(12);
	SET_UP_HANDLER(13);
	SET_UP_HANDLER(14);
	SET_UP_HANDLER(15);

#undef SET_UP_HANDLER

	// Fills in the IDT descriptor
	u32 tableTotalSize = sizeof(InterruptDescriptorTableEntry) * 256;
	u32 tableOffset = ((interruptDescriptorTableAddress & 0xffff) << 16);
	interruptDescriptorTablePointer[0] = tableTotalSize + tableOffset;
	interruptDescriptorTablePointer[1] = interruptDescriptorTableAddress >> 16;

	asm volatile("lidt %0" : : "m"(interruptDescriptorTablePointer));
	asm volatile("sti" ::);
}
