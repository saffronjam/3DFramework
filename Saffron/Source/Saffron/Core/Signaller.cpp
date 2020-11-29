#include "SaffronPCH.h"

#include "Saffron/Core/Signaller.h"

namespace Se
{
Signaller::~Signaller()
{
	m_Signals.Clear();
}

SignalContainer &Signaller::GetSignals()
{
	return m_Signals;
}

}