#pragma once

#include <cstdio>

#include "NormalLock.h"

#define LogTrace(...) Log::Custom(Log::Level::Trace, __FILE__, __LINE__, __VA_ARGS__)
#define LogDebug(...) Log::Custom(Log::Level::Debug, __FILE__, __LINE__, __VA_ARGS__)
#define LogInfo(...) Log::Custom(Log::Level::Info, __FILE__, __LINE__, __VA_ARGS__)
#define LogWarning(...) Log::Custom(Log::Level::Warning, __FILE__, __LINE__, __VA_ARGS__)
#define LogError(...) Log::Custom(Log::Level::Error, __FILE__, __LINE__, __VA_ARGS__)
#define LogFatal(...) Log::Custom(Log::Level::Fatal, __FILE__, __LINE__, __VA_ARGS__)

#define LogTraceUser(...) Log::CustomUser(Log::Level::Trace, __FILE__, __LINE__, __VA_ARGS__)
#define LogDebugUser(...) lLog::CustomUser(Log::Level::Debug, __FILE__, __LINE__, __VA_ARGS__)
#define LogInfoUser(...) Log::CustomUser(Log::Level::Info, __FILE__, __LINE__, __VA_ARGS__)
#define LogWarningUser(...) Log::CustomUser(Log::Level::Warning, __FILE__, __LINE__, __VA_ARGS__)
#define LogErrorUser(...) Log::CustomUser(Log::Level::Error, __FILE__, __LINE__, __VA_ARGS__)
#define LogFatalUser(...) Log::CustomUser(Log::Level::Fatal, __FILE__, __LINE__, __VA_ARGS__)

class Log
{
public:
    enum class Level
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

public:
    static void Custom(Log::Level level, const char *file, int line, const char *fmt, ...);
    static void CustomUser(Log::Level level, const char *file, int line, const char *fmt, ...
    );

    static void SetFilePointer(FILE *filePointer);
    static void SetLevel(Log::Level level);
    static void SetQuiet(bool enable);

private:
    static void ToFile(Log::Level level, struct tm *time, const char *file, int line, const char *fmt, ...);
    static struct tm *GetLocalTime();

private:
    static NormalLock m_lock;
    static FILE *m_filePointer;
    static Log
    ::Level m_level;
    static bool m_isQuiet;
};