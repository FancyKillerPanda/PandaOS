//  ===== Date Created: 12 September, 2020 ===== 

#if !defined(SYSTEM_HPP)
#define SYSTEM_HPP

#if !defined(__clang__)
#error Clang is the only compiler supported right now...
#endif

#define PACKED_STRUCT __attribute__((packed))
#define INTERRUPT_FUNCTION __attribute__((interrupt))

#define STACK_ARRAY_LENGTH(array) (sizeof(array) / sizeof(*array))

#define UNUSED(x) (void) (x)
#define BREAK_POINT() asm volatile("xchg %%bx, %%bx" ::)

#if defined(PANDAOS_DEBUG)
#define ASSERT(x)												\
	if (!(x))													\
	{															\
		log_error("Assertion failed ('%s')\n"					\
				  "       File: %s\n"							\
				  "       Line: %d", #x, __FILE__, __LINE__);	\
		while (true);											\
	}
#else
#define ASSERT(x)
#endif

// Common types
using s8 = signed char;
using s16 = short;
using s32 = int;
using s64 = long long;

using u8 = char; // NOTE(fkp): This is forced to by unsigned through compiler flags
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using usize = u32;

using f32 = float;
using f64 = double;

// Variadic arguments
#define va_start(argsPointer, param) __builtin_va_start(argsPointer, param)
#define va_arg(argsPointer, type) __builtin_va_arg(argsPointer, type)
#define va_end(argsPointer) __builtin_va_end(argsPointer)

using va_list = __builtin_va_list;

#endif
