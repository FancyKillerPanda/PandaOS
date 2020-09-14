//  ===== Date Created: 14 September, 2020 ===== 

#if !defined(UTILITY_HPP)
#define UTILITY_HPP

#include <stddef.h>

// Common types
using i8 = signed char;
using i16 = short;
using i32 = int;

using u8 = char; // NOTE(fkp): This is forced to by unsigned through compiler flags
using u16 = unsigned short;
using u32 = unsigned int;
using usize = size_t;

using f32 = float;
using f64 = double;

// Size conversion to bytes
#define kB(num) (num * 1024)
#define MB(num) (kB(num) * 1024)
#define GB(num) (MB(num) * 1024)
#define TB(num) (GB(num) * 1024)

#endif
