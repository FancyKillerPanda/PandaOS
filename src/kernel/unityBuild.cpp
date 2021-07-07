//  ===== Date Created: 02 December, 2020 ===== 

#include "kernel.cpp"

#include "display/textDisplay.cpp"

#include "interrupts/handleExceptions.cpp"
#include "interrupts/handleInterrupts.cpp"
#include "interrupts/interruptDescriptorTable.cpp"

#include "memory/heapAllocator.cpp"
#include "memory/memoryMap.cpp"
#include "memory/operations.cpp"
#include "memory/physicalAllocator.cpp"
#include "memory/registers.cpp"
#include "memory/virtualAllocator.cpp"

#include "system/io.cpp"

#include "utility/log.cpp"
#include "utility/magic.cpp"
