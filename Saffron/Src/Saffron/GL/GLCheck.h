#pragma once

#include "Saffron/Config.h"

#ifdef SE_DEBUG

#define glCheck(expr) do { expr; Se::SaffronGL::CheckError(__FILE__, __LINE__, #expr); } while (false)

#else

#define glCheck(expr) (expr)

#endif

namespace Se
{
class SaffronGL
{
public:
	static void CheckError(const char *file, unsigned int line, const char *expression);
};
}
