#include "Saffron/SaffronPCH.h"
#include "LayerStack.h"

namespace Se
{

LayerStack::~LayerStack()
{
	for ( auto &layer : m_Layers )
	{
		layer->OnDetach();
	}
}

void LayerStack::PushLayer(Ref<Layer> layer)
{
	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
}

void LayerStack::PushOverlay(Ref<Layer> overlay)
{
	m_Layers.emplace_back(overlay);
}

void LayerStack::PopLayer(Ref<Layer> layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
	if ( it != m_Layers.begin() + m_LayerInsertIndex )
	{
		layer->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}

void LayerStack::PopOverlay(Ref<Layer> overlay)
{
	auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
	if ( it != m_Layers.end() )
	{
		overlay->OnDetach();
		m_Layers.erase(it);
	}
}

std::vector<Ref<Layer>>::iterator LayerStack::begin() { return m_Layers.begin(); }

std::vector<Ref<Layer>>::iterator LayerStack::end() { return m_Layers.end(); }

std::vector<Ref<Layer>>::reverse_iterator LayerStack::rbegin() { return m_Layers.rbegin(); }

std::vector<Ref<Layer>>::reverse_iterator LayerStack::rend() { return m_Layers.rend(); }

std::vector<Ref<Layer>>::const_iterator LayerStack::begin() const { return m_Layers.begin(); }

std::vector<Ref<Layer>>::const_iterator LayerStack::end() const { return m_Layers.end(); }

std::vector<Ref<Layer>>::const_reverse_iterator LayerStack::rbegin() const { return m_Layers.rbegin(); }

std::vector<Ref<Layer>>::const_reverse_iterator LayerStack::rend() const { return m_Layers.rend(); }

}