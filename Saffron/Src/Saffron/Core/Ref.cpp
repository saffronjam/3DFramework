#include "Saffron/SaffronPCH.h"
#include "Ref.h"

namespace Se
{
RefCounted::RefCounted()
	:
	m_ReferenceCount(0)
{
}

void RefCounted::IncreaseReferenceCount() const
{
	m_ReferenceCount++;
}

void RefCounted::DecreaseReferenceCount() const
{
	m_ReferenceCount--;
}
}
