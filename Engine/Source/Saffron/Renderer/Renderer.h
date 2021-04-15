#pragma once

#include "Saffron/Math/AABB.h"
#include "Saffron/Renderer/PrimitiveType.h"
#include "Saffron/Renderer/RenderCommandQueue.h"
#include "Saffron/Renderer/RenderPass.h"
#include "Saffron/Renderer/Shader.h"

namespace Se
{
class Material;
class Mesh;
class MaterialInstance;
class ShaderLibrary;

struct LineVertex
{
	Vector3f Position;
	Vector4f Color;
};

class Renderer : public Instansiated<Renderer>
{
public:
	Renderer();

	static void OnGuiRender();

	static void DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest = true);

	// Commands
	static void Clear();
	static void Clear(float r, float g, float b, float a = 1.0f);
	static void SetClearColor(float r, float g, float b, float a);
	static void SetLineThickness(float thickness);
	// For OpenGL
	static void ClearMagenta();

	static void Submit(const RenderCommand& func);
	static void Execute();

	// ~Actual~ Renderer here... TODO: remove confusion later
	static void BeginRenderPass(Shared<RenderPass> renderPass, bool clear = true);
	static void EndRenderPass();

	static void SubmitQuad(Shared<MaterialInstance> material, const Matrix4f& transform = Matrix4f(1.0f));
	static void SubmitLines(LineVertex* vertices, Uint32 count, Shared<Shader> shader);
	static void SubmitMesh(Shared<Mesh> mesh, const Matrix4f& transform,
	                       Shared<MaterialInstance> overrideMaterial = nullptr);
	static void SubmitMesh(Shared<Mesh> mesh, const Matrix4f& transform, Shared<Shader> shader);
	
private:
	static RenderCommandQueue& GetRenderCommandQueue();

private:
	static struct RendererData _data;
};
}
