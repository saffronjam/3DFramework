#pragma once

#include "Saffron/Base.h"

namespace Se
{
class NonCopyable
{
public:
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};
}
