#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"

namespace Se
{
class RenderGraph;
class RenderTarget;

class Renderer
{
public:
	explicit Renderer(const Shared<Window>& window);
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	~Renderer();
	
	void Submit();
	void Execute();

	static Renderer& Instance();

private:
	static Renderer* _instance;

	const Shared<Window>& _window;
};
}
