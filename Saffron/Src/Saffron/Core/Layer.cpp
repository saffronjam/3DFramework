#include "SaffronPCH.h"
#include "Layer.h"

namespace Se {

Layer::Layer(const std::string &name)
	: m_DebugName(name)
{
}

Layer::~Layer()
= default;
}
