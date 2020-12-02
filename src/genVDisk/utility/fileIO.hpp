//  ===== Date Created: 02 December, 2020 ===== 

#if !defined(FILEIO_HPP)
#define FILEIO_HPP

bool read_entire_file(const u8* path, u8** data, usize* size);
usize write_data_as_blocks(FILE* file, const u8* data, usize size, usize minNumberOfBlocks);

#endif
