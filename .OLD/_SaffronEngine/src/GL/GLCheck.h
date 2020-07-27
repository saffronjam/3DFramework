#pragma once

#include "Config.h"

#include "IException.h"

#ifdef SAFFRON_DEBUG

#define glCheck(expr) do { expr; SaffronGL::CheckError(__FILE__, __LINE__, #expr); } while (false)

#else

#define glCheck(expr) (expr)

#endif

class SaffronGL
{
public:
    static void CheckError(const char *file, unsigned int line, const char *expression) noxnd;

    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, const char *expression, const char *errorString) noexcept;
        const char *what() const noexcept override;
        const char *GetType() const noexcept override;
        const char *GetExpression() const noexcept;
        const char *GetErrorString() const noexcept;

    private:
        std::string expression;
        std::string errorString;
    };
};

