#include "Layers/BaseLayer.h"

namespace Se
{

BaseLayer::BaseLayer()
{
}

void BaseLayer::OnAttach(std::shared_ptr<BatchLoader> &loader)
{
}

void BaseLayer::OnDetach()
{
}

void BaseLayer::OnPreFrame()
{
}

void BaseLayer::OnPostFrame()
{
}

void BaseLayer::OnUpdate()
{
}

void BaseLayer::OnGuiRender()
{
	if ( _viewSystem )
	{
		App::Get().OnGuiRender();
	}
}
}
