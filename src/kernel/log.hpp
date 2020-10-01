//  ===== Date Created: 13 September, 2020 ===== 

#if !defined(LOG_HPP)
#define LOG_HPP

#include "system.hpp"

void log_init();

void log_plain(const u8* message);
void log_info_plain(const u8* message);
void log_warning_plain(const u8* message);
void log_error_plain(const u8* message);

void log(const u8* message, ...);
void log_info(const u8* message, ...);
void log_warning(const u8* message, ...);
void log_error(const u8* message, ...);

#endif
