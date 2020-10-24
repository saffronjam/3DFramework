#pragma once

#include "Saffron/Config.h"
#include "Layer.h"


namespace Se {

class LayerStack
{
public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Layer *layer);
	void PushOverlay(Layer *overlay);
	void PopLayer(Layer *layer);
	void PopOverlay(Layer *overlay);

	ArrayList<Layer *>::iterator begin() { return m_Layers.begin(); }
	ArrayList<Layer *>::iterator end() { return m_Layers.end(); }
private:
	ArrayList<Layer *> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};

}