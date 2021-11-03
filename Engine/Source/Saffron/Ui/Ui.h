#pragma once

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <ImGuizmo.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Image.h"
#include "Saffron/Rendering/Bindables/Texture.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Se
{

enum class GizmoControl
{
	Translate,
	Rotate,
	Scale
};

class Ui : public Singleton<Ui>
{
public:
	Ui();
	~Ui() override;

	void BeginFrame();
	void EndFrame();

	static void BeginGizmo(uint viewportWidth, uint viewportHeight);
	
	static void Image(const Texture& texture, const Vector2& size = {0.0f, 0.0f});
	static void Image(const Texture& texture, const Shader& shader, const Vector2& size = {0.0f, 0.0f});
	static void Image(const class Image& image, const Vector2& size = { 0.0f, 0.0f });
	static void Image(const class Image& image, const Shader& shader, const Vector2& size = {0.0f, 0.0f});

	static void Gizmo(Matrix& result, const Matrix& view, const Matrix proj, GizmoControl control);
	
private:
	static ImGuizmo::OPERATION ToImGuizmoOperation(GizmoControl gizmoControl);

private:
	std::vector<std::shared_ptr<const Shader>> _pendingShaders;

	// Custom sampler data

};
}
