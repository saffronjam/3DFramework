#include "Log.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <ctime>

NormalLock Log::m_lock;
FILE *Log::m_filePointer = nullptr;
Log::Level Log::m_level = Log::Level::Info;
bool Log::m_isQuiet = false;

static const char *level_names[] = {
        "Trace", "Debug", "Info", "Warn", "Error", "Fatal"};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
        "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

void Log::Custom(Log::Level level, const char *file, int line, const char *fmt, ...)
{
    if (level < m_level)
    {
        return;
    }

    /* Acquire lock */
    m_lock.Lock();

    /* Get current time */
    auto time = GetLocalTime();

    /* Log to stderr */
    if (!m_isQuiet)
    {
        va_list args;
        char buf[16];
        buf[strftime(buf, sizeof(buf), "%H:%M:%S", time)] = '\0';
#ifdef LOG_USE_COLOR
        fprintf(stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buf, level_colors[static_cast<int>(level)], level_names[static_cast<int>(level)], file, line);
#else
        fprintf(stderr, "%s %-5s %s:%d: ", buf, level_names[static_cast<int>(level)], file, line);
#endif
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    /* Log to file */
    ToFile(level, time, file, line, fmt);

    /* Release lock */
    m_lock.Unlock();
}

void Log::CustomUser(Log::Level level, const char *file, int line, const char *fmt, ...)
{
    if (level < m_level)
    {
        return;
    }

    /* Acquire lock */
    m_lock.Lock();

    /* Get current time */
    auto time = GetLocalTime();

    /* Log to stderr */
    if (!m_isQuiet)
    {
        va_list args;
        char buf[16];
        buf[strftime(buf, sizeof(buf), "%H:%M:%S", time)] = '\0';
#ifdef LOG_USE_COLOR
        fprintf(stderr, "%s %s%-5s\x1b[0m \x1b:\x1b[0m ", buf, level_colors[static_cast<int>(level)], level_names[static_cast<int>(level)]);
#else
        fprintf(stderr, "%s %s: ", buf, level_names[static_cast<int>(level)]);
#endif
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    /* Log to file */
    ToFile(level, time, file, line, fmt);

    /* Release lock */
    m_lock.Unlock();
}

void Log::SetFilePointer(FILE *filePointer)
{
    m_filePointer = filePointer;
}

void Log::SetLevel(Log::Level level)
{
    m_level = level;
};

void Log::SetQuiet(bool enable)
{
    m_isQuiet = enable;
}

void Log::ToFile(Log::Level level, tm *time, const char *file, int line, const char *fmt, ...)
{
    if (m_filePointer)
    {
        va_list args;
        char buf[32];
        buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", time)] = '\0';
        fprintf(m_filePointer, "%s %-5s: ", buf, level_names[static_cast<int>(level)]);
        va_start(args, fmt);
        vfprintf(m_filePointer, fmt, args);
        va_end(args);
        fprintf(m_filePointer, "\n");
        fflush(m_filePointer);
    }
}

struct tm *Log::GetLocalTime()
{
    time_t t = time(nullptr);
    return localtime(&t);
};
