#pragma once

#include "Saffron/Core/Math/AABB.h"
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

class Renderer
{
public:
	static void Init();

	static void OnGuiRender();

	static void DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest = true);

	// Commands
	static void Clear();
	static void Clear(float r, float g, float b, float a = 1.0f);
	static void SetClearColor(float r, float g, float b, float a);
	static void SetLineThickness(float thickness);
	// For OpenGL
	static void ClearMagenta();

	static void Submit(const RenderCommand &func);
	static void Execute();

	// ~Actual~ Renderer here... TODO: remove confusion later
	static void BeginRenderPass(std::shared_ptr<RenderPass> renderPass, bool clear = true);
	static void EndRenderPass();

	static void SubmitQuad(std::shared_ptr<MaterialInstance> material, const Matrix4f &transform = Matrix4f(1.0f));
	static void SubmitLines(LineVertex *vertices, Uint32 count, std::shared_ptr<Shader> shader);
	static void SubmitMesh(std::shared_ptr<Mesh> mesh, const Matrix4f &transform, std::shared_ptr<MaterialInstance> overrideMaterial = nullptr);
	static void SubmitMesh(std::shared_ptr<Mesh> mesh, const Matrix4f &transform, std::shared_ptr<Shader> shader);

private:
	static RenderCommandQueue &GetRenderCommandQueue();
	static struct RendererData s_Data;
};
}
