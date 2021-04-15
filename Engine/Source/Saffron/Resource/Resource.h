#pragma once

#include "Saffron/Core/MemManaged.h"

namespace Se
{
class Resource : public MemManaged<Resource>
{
public:
	virtual	size_t GetIdentifier() { return 0; }
	virtual ~Resource() = default;

	void Initialize() { if ( m_Initializer.has_value() ) m_Initializer.value()(); };

protected:
	Optional<Function<void()>> m_Initializer;

};
}