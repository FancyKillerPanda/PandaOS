//  ===== Date Created: 15 December, 2020 ===== 

#include "interrupts/handleInterrupts.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "system/common.hpp"

struct PACKED_STRUCT IDTEntry
{
	u16 offsetLow = 0;
	u16 selector = 0;
	u8 unused = 0;
	u8 type = 0;
	u16 offsetHigh = 0;
};
static_assert(sizeof(IDTEntry) == 8,
			  "IDTEntry must be 8 bytes in size.");

constexpr u32 IDT_ADDRESS = 0x7100;
IDTEntry* idtEntries =  (IDTEntry*) IDT_ADDRESS;

// Reprograms the PIC to stop it using the default interrupt
// service routines.
void remap_pic()
{
	constexpr u8 MASTER_PIC_COMMAND = 0x20;
	constexpr u8 MASTER_PIC_DATA = 0x21;
	constexpr u8 SLAVE_PIC_COMMAND = 0xa0;
	constexpr u8 SLAVE_PIC_DATA = 0xa1;

	constexpr u8 INIT_SEQUENCE_START = 0x11;
	constexpr u8 MASTER_PIC_OFFSET = 0x20;
	constexpr u8 SLAVE_PIC_OFFSET = 0x28;
	constexpr u8 MODE_8086 = 0x01;
	
	// NOTE(fkp): The waits make sure this doesn't happen too quickly
	port_out_8(MASTER_PIC_COMMAND, INIT_SEQUENCE_START);
	port_wait();
	port_out_8(SLAVE_PIC_COMMAND, INIT_SEQUENCE_START);
	port_wait();
	port_out_8(MASTER_PIC_DATA, MASTER_PIC_OFFSET);
	port_wait();
	port_out_8(SLAVE_PIC_DATA, SLAVE_PIC_OFFSET);
	port_wait();
	port_out_8(MASTER_PIC_DATA, 0x04); // Tells master there's a slave on IRQ2
	port_wait();
	port_out_8(SLAVE_PIC_DATA, 0x02); // Tells slave its cascade identity
	port_wait();
	port_out_8(MASTER_PIC_DATA, MODE_8086);
	port_wait();
	port_out_8(SLAVE_PIC_DATA, MODE_8086);
	port_wait();
	port_out_8(MASTER_PIC_DATA, 0x00);
	port_wait();
	port_out_8(SLAVE_PIC_DATA, 0x00);
	port_wait();
}

void set_up_idt_entries()
{
	constexpr u16 KERNEL_CODE_SEGEMENT_SELECTOR = 0x18;
	constexpr u16 INTERRUPT_GATE = 0x8e;
	
#define SET_UP_HANDLER(index, function)									\
	IDTEntry& entry##index = idtEntries[index];							\
	entry##index.offsetLow = (u16) (((u32) function) & 0x0000ffff);		\
	entry##index.selector = KERNEL_CODE_SEGEMENT_SELECTOR;				\
	entry##index.unused = 0;											\
	entry##index.type = INTERRUPT_GATE;									\
	entry##index.offsetHigh = (u16) ((((u32) function) & 0xffff0000) >> 16);

	SET_UP_HANDLER(32, handle_interrupt_request_0);
	SET_UP_HANDLER(33, handle_interrupt_request_1);
	SET_UP_HANDLER(34, handle_interrupt_request_2);
	SET_UP_HANDLER(35, handle_interrupt_request_3);
	SET_UP_HANDLER(36, handle_interrupt_request_4);
	SET_UP_HANDLER(37, handle_interrupt_request_5);
	SET_UP_HANDLER(38, handle_interrupt_request_6);
	SET_UP_HANDLER(39, handle_interrupt_request_7);
	SET_UP_HANDLER(40, handle_interrupt_request_8);
	SET_UP_HANDLER(41, handle_interrupt_request_9);
	SET_UP_HANDLER(42, handle_interrupt_request_10);
	SET_UP_HANDLER(43, handle_interrupt_request_11);
	SET_UP_HANDLER(44, handle_interrupt_request_12);
	SET_UP_HANDLER(45, handle_interrupt_request_13);
	SET_UP_HANDLER(46, handle_interrupt_request_14);
	SET_UP_HANDLER(47, handle_interrupt_request_15);
}

void load_idt()
{
	struct PACKED_STRUCT IDTPointer
	{
		u16 size = 0;
		u32 address = 0;
	};
	static_assert(sizeof(IDTPointer) == 6, "Size of IDTPointer must be 6 bytes.");

	IDTPointer idtPointer;
	idtPointer.size  = sizeof(IDTEntry) * 256;
	idtPointer.address = IDT_ADDRESS;

	asm volatile("lidt %0" : : "m"(idtPointer));
	asm volatile("sti" ::);
}

void init_interrupt_descriptor_table()
{
	remap_pic();
	set_up_idt_entries();
	load_idt();
}
