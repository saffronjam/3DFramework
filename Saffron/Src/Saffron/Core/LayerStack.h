#pragma once

#include "Saffron/Config.h"
#include "Layer.h"

namespace Se {

class LayerStack : public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<Layer *> OnPushLayer;
		static SignalAggregate<Layer *> OnPushOverlay;
		static SignalAggregate<Layer *> OnPopLayer;
		static SignalAggregate<Layer *> OnPopOverlay;
	};

public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Layer *layer, Shared<BatchLoader> &batchLoader);
	void PushOverlay(Layer *overlay, Shared<BatchLoader> &batchLoader);
	void PopLayer(int count);
	void PopOverlay(int count);
	void EraseLayer(Layer *layer);
	void EraseOverlay(Layer *overlay);

	void Clear();
	Layer *Front();
	Layer *Back();
	ArrayList<Layer *>::iterator begin() { return m_Layers.begin(); }
	ArrayList<Layer *>::iterator end() { return m_Layers.end(); }

private:
	ArrayList<Layer *> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};

}