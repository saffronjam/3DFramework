#pragma once

#define LOG_EXCEPTION

#define THROW(ExceptionClass, msg, format...)          \
    {                                                  \
        char buf[200] = {0};                           \
        sprintf(buf, msg, format);                     \
        throw ExceptionClass(__LINE__, __FILE__, buf); \
    }

#define LogWhat LogWarning("\n%s", e.what())
#define LogNoDetails LogWarning("\n%s", "No details available");

#ifdef LOG_EXCEPTION

#include "Log.h"

#define LogOnly                           \
    catch (const IException &e)           \
    {                                     \
        LogWarning("\n%s", e.what());       \
    }                                     \
    catch (const std::exception &e)       \
    {                                     \
        LogWarning("\n%s", e.what());       \
    }                                     \
    catch (...)                           \
    {                                     \
        LogWarning("No details available"); \
    }
#else
#define LogOnly                     \
    catch (const IException &e)     \
    {                               \
    }                               \
    catch (const std::exception &e) \
    {                               \
    }                               \
    catch (...)                     \
    {                               \
    }
#endif