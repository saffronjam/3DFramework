#include "SaffronPCH.h"

#include "Saffron/Rendering/Renderer.h"
#include <Windows.h>

#include "Saffron/Common/App.h"

namespace Se
{
Renderer::Renderer(const Window& window) :
	Singleton(this),
	_bindableStore(std::make_unique<BindableStore>())
{
	BeginQueue("Main");
}

Renderer::~Renderer()
{
	EndQueue();
}

auto Renderer::Config() -> RendererConfig&
{
	return Instance()._config;
}

void Renderer::Submit(const RenderFn& fn, std::source_location location)
{
	if (auto& inst = Instance(); !inst._activeContainer->Executing && inst._activeStrategy == RenderStrategy::Deferred)
	[[likely]]
	{
		inst._activeContainer->Submitions.emplace_back(Submition{fn, location});
	}
	else
	{
		inst.ExecuteSubmition(Submition{fn, location});
	}
}

void Renderer::Execute()
{
	if (_activeContainer == nullptr)
	{
		throw SaffronException("Call to Execute with out am active render queue");
	}

	auto* executingContainer = _activeContainer;

	executingContainer->Executing = true;
	for (auto& submition : executingContainer->Submitions)
	{
		ExecuteSubmition(submition);
	}
	executingContainer->Executing = false;
	executingContainer->Submitions.clear();
}

void Renderer::BeginFrame()
{
	SetRenderState(RenderState::Default);
}

void Renderer::EndFrame()
{
}

void Renderer::BeginQueue(const std::string name)
{
	_queues.push(name);
	const auto findResult = _submitionContainers.find(name);
	if (findResult == _submitionContainers.end())
	{
		const auto result = _submitionContainers.emplace(name, RenderQueue{name});
		_activeContainer = &result.first->second;
	}
	else
	{
		_activeContainer = &findResult->second;
	}
}

void Renderer::EndQueue()
{
	Debug::Assert(!_activeContainer->Executing, "Can't end a render queue if the active queue is executing");

	if (_queues.size() == 0)
	{
		throw SaffronException("No queue to end.");
	}
	_queues.pop();

	if (_queues.size() == 0)
	{
		_activeContainer = nullptr;
	}
	else
	{
		const auto findResult = _submitionContainers.find(_queues.top());
		Debug::Assert(findResult != _submitionContainers.end());
		_activeContainer = &findResult->second;
	}
}

void Renderer::BeginStrategy(RenderStrategy strategy)
{
	auto& inst = Instance();
	inst._strategies.push(strategy);
	inst._activeStrategy = strategy;
}

void Renderer::EndStrategy()
{
	auto& inst = Instance();
	if (inst._strategies.size() == 0)
	{
		throw SaffronException("No strategy to end.");
	}
	inst._strategies.pop();

	if (inst._strategies.size() == 0)
	{
		inst._activeStrategy = RenderStrategy::Deferred;
	}
	else
	{
		inst._activeStrategy = inst._strategies.top();
	}
}

void Renderer::SubmitIndexed(uint indexCount, uint baseIndex, uint baseVertex)
{
}

void Renderer::SubmitFullscreenQuad()
{
}

void Renderer::Log(const std::string& string)
{
	Submit([string](const RendererPackage& package)
	{
		{
			std::cout << "[Renderer] " << string << '\n';
		}
	});
}

void Renderer::SetRenderState(RenderState state)
{
}

void Renderer::ResetRenderState()
{
	SetRenderState(RenderState::Default);
}

void Renderer::SetViewportSize(uint width, uint height, float depth)
{
	SetViewportSize(0, 0, width, height, depth);
}

void Renderer::SetViewportSize(uint x, uint y, uint width, uint height, float depth)
{
}

auto Renderer::WhiteTexture() -> const std::shared_ptr<Texture>&
{
	return Instance()._whiteTexture;
}

void Renderer::ExecuteSubmition(const Submition& submition)
{
	try
	{
	}
	catch (const SaffronException& e)
	{
		MessageBoxA(static_cast<HWND>(App::Instance().Window().NativeHandle()), e.What().c_str(), e.Type(),
		            MB_OK | MB_ICONEXCLAMATION);
	}
}


void Renderer::CreateRenderState(uint state)
{
}
}
