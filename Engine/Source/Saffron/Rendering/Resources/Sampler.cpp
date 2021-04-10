#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/Sampler.h"

namespace Se
{
Sampler::Sampler(String name) :
	Uniform(Move(name), UniformType::Sampler, 1)
{
}

Sampler::Ptr Sampler::Create(String name)
{
	return CreateShared<Sampler>(Move(name));
}
}
