//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(SYSTEM_HPP)
#define SYSTEM_HPP

#define UNUSED(x) (void) (x)
#define BREAK_POINT() asm volatile("xchg %%bx, %%bx" ::)
#define STACK_ARRAY_LENGTH(array) (sizeof(array) / sizeof(*array))

// Common types
using s8 = signed char;
using s16 = short;
using s32 = int;
using s64 = long long;

using u8 = char; // NOTE(fkp): Forced to by unsigned through compiler flags
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using usize = u32;

using f32 = float;
using f64 = double;

#endif
