//  ===== Date Created: 23 July, 2021 ===== 

#include "memory/registers.hpp"
#include "multitasking/scheduler.hpp"

constexpr u32 TSS_ADDRESS = 0xc0007500;
constexpr u32 TSS_ESP_OFFSET = 4;

extern "C" void jump_to_usermode(void* function);

void test_usermode_process()
{
	BREAK_POINT();
	while (true);
}

void switch_process()
{
	RegisterState currentRegisters;
	get_registers(&currentRegisters);
	
	printf("Jumping to usermode (return esp: %x)\n", currentRegisters.esp);
	
	// Sets the kernel stack pointer in the TSS
	*((u32*) (TSS_ADDRESS + TSS_ESP_OFFSET)) = currentRegisters.esp;

	// TODO(fkp): UB converting function pointer to void*
	jump_to_usermode((void*) &test_usermode_process);
}
