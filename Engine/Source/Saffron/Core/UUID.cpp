#include "SaffronPCH.h"

#include "Saffron/Core/UUID.h"
#include "Saffron/Math/Random.h"

namespace Se
{
static RandomGenerator<Uint64> sRandom(0, std::numeric_limits<Uint64>::max());


UUID::UUID() :
	m_UUID(sRandom.Generate())
{
}

UUID::UUID(Uint64 uuid) :
	m_UUID(uuid)
{
}

UUID::UUID(const UUID& other) :
	UUID(other.m_UUID)
{
}
}
