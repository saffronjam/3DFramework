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

void LayerStack::PushLayer(Layer::Ptr layer)
{
	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
}

void LayerStack::PushOverlay(Layer::Ptr overlay)
{
	m_Layers.emplace_back(overlay);
}

void LayerStack::PopLayer(Layer::Ptr layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
	if ( it != m_Layers.begin() + m_LayerInsertIndex )
	{
		layer->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}

void LayerStack::PopOverlay(Layer::Ptr overlay)
{
	auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
	if ( it != m_Layers.end() )
	{
		overlay->OnDetach();
		m_Layers.erase(it);
	}
}

std::vector<Layer::Ptr>::iterator LayerStack::begin() { return m_Layers.begin(); }

std::vector<Layer::Ptr>::iterator LayerStack::end() { return m_Layers.end(); }

std::vector<Layer::Ptr>::reverse_iterator LayerStack::rbegin() { return m_Layers.rbegin(); }

std::vector<Layer::Ptr>::reverse_iterator LayerStack::rend() { return m_Layers.rend(); }

std::vector<Layer::Ptr>::const_iterator LayerStack::begin() const { return m_Layers.begin(); }

std::vector<Layer::Ptr>::const_iterator LayerStack::end() const { return m_Layers.end(); }

std::vector<Layer::Ptr>::const_reverse_iterator LayerStack::rbegin() const { return m_Layers.rbegin(); }

std::vector<Layer::Ptr>::const_reverse_iterator LayerStack::rend() const { return m_Layers.rend(); }

}