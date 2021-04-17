#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Layer.h"

namespace Se
{
class LayerStack
{
public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Shared<Layer>, Shared<BatchLoader>& batchLoader);
	void PushOverlay(Shared<Layer>, Shared<BatchLoader>& batchLoader);
	void PopLayer(int count);
	void PopOverlay(int count);
	void EraseLayer(Shared<Layer> layer);
	void EraseOverlay(Shared<Layer> overlay);

	void Clear();
	Shared<Layer> Front();
	Shared<Layer> Back();

	ArrayList<Shared<Layer>>::iterator begin() { return _layers.begin(); }

	ArrayList<Shared<Layer>>::iterator end() { return _layers.end(); }

public:
	EventSubscriberList<Shared<Layer>> PushedLayer;
	EventSubscriberList<Shared<Layer>> PushedOverlay;
	EventSubscriberList<Shared<Layer>> PoppedLayer;
	EventSubscriberList<Shared<Layer>> PoppedOverlay;

private:
	ArrayList<Shared<Layer>> _layers;
	unsigned int _layerInsertIndex = 0;
};
}
