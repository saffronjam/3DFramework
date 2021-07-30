#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/PrimitiveTopology.h"

#include "Saffron/Rendering/Renderer.h"

namespace Se
{
PrimitiveTopology::PrimitiveTopology(PrimitiveTopologyType topology) :
	_nativeTopology(Utils::ToD3D11PrimitiveTopology(topology))
{
}

void PrimitiveTopology::Bind()
{
	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			package.Context.IASetPrimitiveTopology(_nativeTopology);
		}
	);
}
}
