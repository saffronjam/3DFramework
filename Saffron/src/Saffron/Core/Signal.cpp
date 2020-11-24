#include "SaffronPCH.h"

#include "Saffron/Core/Signal.h"

namespace Se
{

Uint32 Signal<void>::m_Serial = 0;

//////////////////////////////////////////////////////
/// Basic Signal
//////////////////////////////////////////////////////

BasicSignal::TypeID BasicSignal::GenerateTypeID()
{
	return {};
}

}
