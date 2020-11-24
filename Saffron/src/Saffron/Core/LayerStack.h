#pragma once

#include "Saffron/Config.h"
#include "Layer.h"

namespace Se {

class LayerStack : public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<Shared<Layer>> OnPushLayer;
		static SignalAggregate<Shared<Layer>> OnPushOverlay;
		static SignalAggregate<Shared<Layer>> OnPopLayer;
		static SignalAggregate<Shared<Layer>> OnPopOverlay;
	};

public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Shared<Layer>, Shared<BatchLoader> &batchLoader);
	void PushOverlay(Shared<Layer>, Shared<BatchLoader> &batchLoader);
	void PopLayer(int count);
	void PopOverlay(int count);
	void EraseLayer(Shared<Layer> layer);
	void EraseOverlay(Shared<Layer> overlay);

	void Clear();
	Shared<Layer> Front();
	Shared<Layer> Back();
	ArrayList<Shared<Layer>>::iterator begin() { return m_Layers.begin(); }
	ArrayList<Shared<Layer>>::iterator end() { return m_Layers.end(); }

private:
	ArrayList<Shared<Layer>> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};

}