#include "SaffronPCH.h"

#include "Saffron/Core/UUID.h"
#include "Saffron/Math/Random.h"

namespace Se
{
static RandomGenerator<ulong> sRandom(0, std::numeric_limits<ulong>::max());


UUID::UUID() :
	_uuid(sRandom.Generate())
{
}

UUID::UUID(ulong uuid) :
	_uuid(uuid)
{
}

UUID::UUID(const UUID& other) :
	UUID(other._uuid)
{
}
}
