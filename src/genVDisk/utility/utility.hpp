//  ===== Date Created: 14 September, 2020 ===== 

#if !defined(UTILITY_HPP)
#define UTILITY_HPP

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

// Size conversion to bytes
#define kB(num) (num * 1024)
#define MB(num) (kB(num) * 1024)
#define GB(num) (MB(num) * 1024)
#define TB(num) (GB(num) * 1024)

// NOTE(fkp): The caller is responsible for calling free()
u8* concat_strings(const u8* strOne, const u8* strTwo);

usize write_data_as_blocks(FILE* file, const u8* data, usize size, usize minNumberOfBlocks = 0);
u16 read_word(const u8* data, usize indexOfFirstByte);

// This function will fill in the data and size parameters if it
// returns true.
bool read_entire_file(const u8* path, u8** data, usize* size);

#endif
