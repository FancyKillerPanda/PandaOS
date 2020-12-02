//  ===== Date Created: 02 December, 2020 ===== 

#if !defined(COMMON_HPP)
#define COMMON_HPP

// Common types
using s8 = signed char;
using s16 = short;
using s32 = int;

using u8 = char; // NOTE(fkp): This is forced to by unsigned through compiler flags
using u16 = unsigned short;
using u32 = unsigned int;
using usize = size_t;

using f32 = float;
using f64 = double;

// Gets the number of elements in a stack array
#define STACK_ARRAY_LENGTH(array) (sizeof(array) / sizeof(*array))

#endif
