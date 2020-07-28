#pragma once

#include "Saffron/Graphics/Layer.h"

namespace Se
{
class LayerStack
{
public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Ref<Layer> layer);
	void PushOverlay(Ref<Layer> overlay);
	void PopLayer(Ref<Layer> layer);
	void PopOverlay(Ref<Layer> overlay);

	std::vector<Ref<Layer>>::iterator begin();
	std::vector<Ref<Layer>>::iterator end();
	std::vector<Ref<Layer>>::reverse_iterator rbegin();
	std::vector<Ref<Layer>>::reverse_iterator rend();

	std::vector<Ref<Layer>>::const_iterator begin() const;
	std::vector<Ref<Layer>>::const_iterator end()	const;
	std::vector<Ref<Layer>>::const_reverse_iterator rbegin() const;
	std::vector<Ref<Layer>>::const_reverse_iterator rend() const;

private:
	std::vector<Ref<Layer>> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};
}
