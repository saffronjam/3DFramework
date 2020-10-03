#pragma once

#include "Saffron/Renderer/Material.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/PrimitiveType.h"
#include "Saffron/Renderer/RenderCommandQueue.h"
#include "Saffron/Renderer/RenderPass.h"

namespace Se
{
class ShaderLibrary;

class Renderer
{
public:
	static void Init();

	static void OnImGuiRender();

	static void DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest = true);

	// Commands
	static void Clear();
	static void Clear(float r, float g, float b, float a = 1.0f);
	static void SetClearColor(float r, float g, float b, float a);
	static void SetLineThickness(float thickness);
	// For OpenGL
	static void ClearMagenta();


	static Ref<ShaderLibrary> GetShaderLibrary();

	template<typename FuncT>
	static void Submit(FuncT &&func);
	static void WaitAndRender();

	// ~Actual~ Renderer here... TODO: remove confusion later
	static void BeginRenderPass(Ref<RenderPass> renderPass, bool clear = true);
	static void EndRenderPass();

	static void SubmitQuad(Ref<MaterialInstance> material, const glm::mat4 &transform = glm::mat4(1.0f));
	static void SubmitFullscreenQuad(Ref<MaterialInstance> material);
	static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4 &transform, Ref<MaterialInstance> overrideMaterial = nullptr);

	static void DrawAABB(const AABB &aabb, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(1.0f));
	static void DrawAABB(Ref<Mesh> mesh, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(1.0f));

private:
	static RenderCommandQueue &GetRenderCommandQueue();
};

template <typename FuncT>
void Renderer::Submit(FuncT &&func)
{
	auto renderCmd = [](void *ptr)
	{
		auto pFunc = *static_cast<FuncT *>(ptr);
		pFunc();

		// NOTE: Instead of destroying we could try and enforce all items to be trivially destructible
		// however some items like uniforms which contain std::strings still exist for now
		// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
		pFunc.~FuncT();
	};
	auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
	new(storageBuffer) FuncT(std::forward<FuncT>(func));
}
}
