//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(LOG_HPP)
#define LOG_HPP

#include "system/common.hpp"

void log(const u8* message, ...);
void log_info(const u8* message, ...);
void log_warning(const u8* message, ...);
void log_error(const u8* message, ...);

#endif
