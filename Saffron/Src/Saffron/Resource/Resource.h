#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Resource : public ReferenceCounted
{
public:
	virtual	size_t GetIdentifier() = 0;
	virtual ~Resource() = default;
};
}