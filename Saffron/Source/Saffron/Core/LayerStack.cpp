#include "SaffronPCH.h"

#include "Saffron/Core/LayerStack.h"

namespace Se
{

SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPushLayer;
SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPushOverlay;
SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPopLayer;
SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPopOverlay;

LayerStack::~LayerStack()
{
	for ( std::shared_ptr<Layer> layer : m_Layers )
	{
		layer->OnDetach();
	}
	m_Layers.clear();
}

void LayerStack::PushLayer(std::shared_ptr<Layer> layer, std::shared_ptr<BatchLoader> &batchLoader)
{
	auto newLayer = *m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
	GetSignals().Emit(Signals::OnPushOverlay, newLayer);
	newLayer->OnAttach(batchLoader);
}

void LayerStack::PushOverlay(std::shared_ptr<Layer> overlay, std::shared_ptr<BatchLoader> &batchLoader)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
	auto newOverlay = m_Layers.emplace_back(overlay);
	GetSignals().Emit(Signals::OnPushOverlay, newOverlay);
}

void LayerStack::PopLayer(int count)
{
	for ( int i = 0; i < count; i++ )
	{
		GetSignals().Emit(Signals::OnPopLayer, m_Layers.back());
		m_Layers.back()->OnDetach();
		m_Layers.pop_back();
		m_LayerInsertIndex--;
	}
}

void LayerStack::PopOverlay(int count)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
	for ( int i = 0; i < count; i++ )
	{
		GetSignals().Emit(Signals::OnPopOverlay, m_Layers.back());
		m_Layers.back()->OnDetach();
		m_Layers.pop_back();
	}
}

void LayerStack::EraseLayer(std::shared_ptr<Layer> layer)
{
	const auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if ( it != m_Layers.end() )
	{
		GetSignals().Emit(Signals::OnPopOverlay, *it);
		(*it)->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}

void LayerStack::EraseOverlay(std::shared_ptr<Layer> overlay)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
	const auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
	if ( it != m_Layers.end() )
	{
		m_Layers.erase(it);
	}
}

void LayerStack::Clear()
{
	for ( auto &layer : m_Layers )
	{
		layer->OnDetach();
	}
	m_Layers.clear();
}

std::shared_ptr<Layer> LayerStack::Front()
{
	return m_Layers.front();
}

std::shared_ptr<Layer> LayerStack::Back()
{
	return m_Layers.back();
}
}
