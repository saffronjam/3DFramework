#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Uniform.h"

namespace Se
{
class Sampler : public Uniform
{
public:
	using Ptr = Shared<Sampler>;

public:
	explicit Sampler(String name);

	static Ptr Create(String name);

private:
};
}
