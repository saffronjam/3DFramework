#pragma once

#include "Saffron/Math/AABB.h"
#include "Saffron/Rendering/PrimitiveType.h"
#include "Saffron/Rendering/RenderCommandQueue.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
class Material;
class Mesh;
class MaterialInstance;
class ShaderLibrary;

struct LineVertex
{
	Vector3 Position;
	Vector4 Color;
};

class Renderer : public SingleTon<Renderer>, public Managed
{
public:
	Renderer();
	~Renderer();

	static void OnGuiRender();

	static void LoadRequiredAssets();
	
	// Commands
	static void Clear();
	static void Clear(float r, float g, float b, float a = 1.0f);
	static void SetClearColor(float r, float g, float b, float a);

	static void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true);

	// For OpenGL
	static void SetLineThickness(float thickness);
	static void ClearMagenta();

	template <typename FuncT>
	static void Submit(FuncT&& func)
	{
		auto renderCmd = [](void* ptr)
		{
			auto pFunc = static_cast<FuncT*>(ptr);
			(*pFunc)();

			// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
			// however some items like uniforms which contain std::strings still exist for now
			// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
			pFunc->~FuncT();
		};
		auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
		new(storageBuffer) FuncT(std::forward<FuncT>(func));
	}

	static void WaitAndRender();

	static void Begin(Shared<Framebuffer> framebuffer, bool clear = true);
	static void End();

	static void SubmitQuad(Shared<MaterialInstance> material, const Matrix4& transform = Matrix4(1.0f));
	static void SubmitFullscreenQuad(Shared<MaterialInstance> material);
	static void SubmitMesh(Shared<Mesh> mesh, const Matrix4& transform,
	                       Shared<MaterialInstance> overrideMaterial = nullptr);
	static void SubmitMeshWithShader(Shared<Mesh> mesh, const Matrix4& transform, Shared<Shader> shader);

	static void DrawAABB(const AABB& aabb, const Matrix4& transform, const Vector4& color = Vector4(1.0f));
	static void DrawAABB(Shared<Mesh> mesh, const Matrix4& transform, const Vector4& color = Vector4(1.0f));

private:
	static RenderCommandQueue& GetRenderCommandQueue();

private:
	struct RendererData* _data;
};
}
