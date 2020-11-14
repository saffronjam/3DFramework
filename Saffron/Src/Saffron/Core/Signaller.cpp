#include "SaffronPCH.h"

#include "Saffron/Core/Signaller.h"

namespace Se
{
SignalContainer &Signaller::GetSignals()
{
	return m_Signals;
}

}