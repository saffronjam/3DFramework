#include "SaffronPCH.h"

#include "Saffron/Core/LayerStack.h"

namespace Se
{
LayerStack::~LayerStack()
{
	for (Shared<Layer> layer : _layers)
	{
		layer->OnDetach();
	}
	_layers.clear();
}

void LayerStack::PushLayer(Shared<Layer> layer, Shared<BatchLoader>& batchLoader)
{
	auto newLayer = *_layers.emplace(_layers.begin() + _layerInsertIndex, layer);
	_layerInsertIndex++;
	PushedLayer.Invoke(layer);
	newLayer->OnAttach(batchLoader);
}

void LayerStack::PushOverlay(Shared<Layer> overlay, Shared<BatchLoader>& batchLoader)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
}

void LayerStack::PopLayer(int count)
{
	for (int i = 0; i < count; i++)
	{
		PoppedLayer.Invoke(_layers.back());
		_layers.back()->OnDetach();
		_layers.pop_back();
		_layerInsertIndex--;
	}
}

void LayerStack::PopOverlay(int count)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
}

void LayerStack::EraseLayer(Shared<Layer> layer)
{
	const auto it = std::find(_layers.begin(), _layers.end(), layer);
	if (it != _layers.end())
	{
		PoppedLayer.Invoke(*it);
		(*it)->OnDetach();
		_layers.erase(it);
		_layerInsertIndex--;
	}
}

void LayerStack::EraseOverlay(Shared<Layer> overlay)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
}

void LayerStack::Clear()
{
	for (auto& layer : _layers)
	{
		layer->OnDetach();
	}
	_layers.clear();
}

Shared<Layer> LayerStack::Front()
{
	return _layers.front();
}

Shared<Layer> LayerStack::Back()
{
	return _layers.back();
}
}
