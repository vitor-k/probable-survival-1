#ifndef LOG_H
#define LOG_H

#include<string>
#include<stdarg.h>

#include<fmt/core.h>
#include<fmt/os.h>

enum class LogLevel {
    Debug,
    Normal
};

extern fmt::ostream out;

template<typename... T>
inline void log(LogLevel level, const char* s, T&&... args) {
    if(level == LogLevel::Debug) {
#ifdef _debug
        out.print(s, args...);
        fmt::print(s, args...);
#endif
    }
    else {
        out.print(s, args...);
        fmt::print(s, args...);
    }
}

#define LOG(...) log(::LogLevel::Normal, __VA_ARGS__)

#ifdef _debug
#define LOG_DEBUG(...) log(::LogLevel::Debug, __VA_ARGS__)
#else
#define LOG_DEBUG(...) (void(0))
#endif

#endif //LOG_H
