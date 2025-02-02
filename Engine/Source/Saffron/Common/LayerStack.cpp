﻿#include "SaffronPCH.h"

#include "Saffron/Common/LayerStack.h"

namespace Se
{
void LayerStack::Attach(std::shared_ptr<Layer> layer)
{
	auto ref = _layers.emplace_back(std::move(layer));
	ref->OnAttach();
}

void LayerStack::Detach(std::shared_ptr<Layer> layer)
{
	layer->OnDetach();
	std::erase(_layers, layer);
}

void LayerStack::Clear()
{
	_layers.clear();
}

auto LayerStack::begin() const -> std::vector<std::shared_ptr<Layer>>::const_iterator
{
	return _layers.begin();
}

auto LayerStack::end() const -> std::vector<std::shared_ptr<Layer>>::const_iterator
{
	return _layers.end();
}

auto LayerStack::begin() -> std::vector<std::shared_ptr<Layer>>::iterator
{
	return _layers.begin();
}

auto LayerStack::end() -> std::vector<std::shared_ptr<Layer>>::iterator
{
	return _layers.end();
}
}
