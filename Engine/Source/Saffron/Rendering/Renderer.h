#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"

namespace Se
{
class RenderGraph;
class RenderTarget;
class Mesh;
class Program;
class Material;

class Renderer
{
public:
	explicit Renderer(const Shared<Window>& window);
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	~Renderer();
	
	void SetViewId(Uint16 id);
	void SetViewSize(Uint16 width, Uint16 height);
	void SetViewProjection(const Matrix& view, const Matrix& projection);
	
	void Submit(const Shared<Mesh>& mesh, const Shared<Program>& program, const Shared<Material>& material);
	void Execute();

	static Renderer& Instance();

private:
	bool OnRendertargetResize(Uint32 width, Uint32 height);
	bool OnWindowResize(const WindowResizedEvent& event);

private:
	static Renderer* _instance;

	const Shared<Window>& _window;

	Uint16 _viewId = 0;
	Matrix _view = Matrix::Identity, _perspective = Matrix::Identity;
};
}
