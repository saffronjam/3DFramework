#include "SaffronPCH.h"
#include "Layer.h"

namespace Se {

Layer::Layer(const std::string &debugName)
	: m_DebugName(debugName)
{
}

Layer::~Layer()
= default;
}
