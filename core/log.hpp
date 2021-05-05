#ifndef LOG_HPP
#define LOG_HPP

#include "string.hpp"

enum LogLevel {
    LL_Debug,
    LL_Info,
    LL_Warning,
    LL_Error
};

void log(String content, LogLevel level);

#endif

