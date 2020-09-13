//  ===== Date Created: 13 September, 2020 ===== 

#if !defined(LOG_HPP)
#define LOG_HPP

#include "system.hpp"

void log_init();

void log_info(const u8* message);
void log_warning(const u8* message);
void log_error(const u8* message);

#endif
