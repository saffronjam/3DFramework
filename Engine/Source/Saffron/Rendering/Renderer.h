#pragma once

#include <source_location>
#include <vector>
#include <unordered_map>
#include <stack>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Graphics/ModelRegistry.h"
#include "Saffron/Rendering/BindableStore.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/RenderState.h"

namespace Se
{
using HRESULT = long;

struct RendererPackage
{
};

enum class RenderStrategy
{
	Immediate,
	Deferred
};

using RenderFn = std::function<void(const RendererPackage& package)>;


struct Submition
{
	RenderFn Fn;
	std::source_location Location;
};

struct RenderQueue
{
	std::string Name;
	std::vector<Submition> Submitions;
	bool Executing = false;
};

struct RendererConfig
{
	uint EnvironmentMapResolution = 1024;
	uint IrradianceMapComputeSamples = 512;
};


class Model;
struct Mvp;
class TransformCBuffer;

class VertexBuffer;
class IndexBuffer;
class InputLayout;

class Renderer : public Singleton<Renderer>
{
public:
	explicit Renderer(const Window& window);
	~Renderer() override;

	static auto Config() -> RendererConfig&;

	static void Submit(const RenderFn& fn, std::source_location location = std::source_location::current());

	void Execute();

	void BeginFrame();
	void EndFrame();

	void BeginQueue(const std::string name);
	void EndQueue();

	static void BeginStrategy(RenderStrategy strategy);
	static void EndStrategy();

	static void SubmitIndexed(uint indexCount, uint baseIndex = 0, uint baseVertex = 0);
	static void SubmitFullscreenQuad();

	template <typename ... Args>
	static void Log(const std::string& string, const Args& ... args);
	static void Log(const std::string& string);

	static void SetRenderState(RenderState state);
	static void ResetRenderState();
	static void SetViewportSize(uint width, uint height, float depth = 1.0f);
	static void SetViewportSize(uint x, uint y, uint width, uint height, float depth = 1.0f);

	static auto WhiteTexture() -> const std::shared_ptr<Texture>&;

private:
	void ExecuteSubmition(const Submition& submition);

private:
	void CreateRenderState(uint state);

private:
	RendererConfig _config;

	// Render state
	RenderState _submittedState = 0;

	// Renderer data
	std::unique_ptr<BindableStore> _bindableStore;
	std::unique_ptr<ModelRegistry> _meshStore;
	std::shared_ptr<RenderGraph> _currentRenderGraph;

	// Prepared submitions

	// Submitions
	std::map<std::string, RenderQueue> _submitionContainers;
	RenderQueue* _activeContainer = nullptr;
	std::stack<std::string> _queues;

	// Strategy
	RenderStrategy _activeStrategy = RenderStrategy::Deferred;
	std::stack<RenderStrategy> _strategies;

	// Prepare textures
	std::shared_ptr<Texture> _whiteTexture;
};

template <typename ... Args>
void Renderer::Log(const std::string& string, const Args&... args)
{
	Log(std::format(string, args...));
}

}
