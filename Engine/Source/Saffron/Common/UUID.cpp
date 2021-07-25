#include "SaffronPCH.h"

#include "Saffron/Common/Uuid.h"
#include "Saffron/Math/Random.h"

namespace Se
{
static RandomGenerator<ulong> sRandom(0, std::numeric_limits<ulong>::max());


Uuid::Uuid() :
	_uuid(sRandom.Generate())
{
}

Uuid::Uuid(ulong uuid) :
	_uuid(uuid)
{
}

Uuid::Uuid(const Uuid& other) :
	Uuid(other._uuid)
{
}
}
