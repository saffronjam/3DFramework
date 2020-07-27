#pragma once

#include "Saffron/Graphics/Layer.h"

namespace Se
{
class LayerStack
{
public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Layer::Ptr layer);
	void PushOverlay(Layer::Ptr overlay);
	void PopLayer(Layer::Ptr layer);
	void PopOverlay(Layer::Ptr overlay);

	std::vector<Layer::Ptr>::iterator begin();
	std::vector<Layer::Ptr>::iterator end();
	std::vector<Layer::Ptr>::reverse_iterator rbegin();
	std::vector<Layer::Ptr>::reverse_iterator rend();

	std::vector<Layer::Ptr>::const_iterator begin() const;
	std::vector<Layer::Ptr>::const_iterator end()	const;
	std::vector<Layer::Ptr>::const_reverse_iterator rbegin() const;
	std::vector<Layer::Ptr>::const_reverse_iterator rend() const;

private:
	std::vector<Layer::Ptr> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};
}
