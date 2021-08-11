#pragma once

#include <imgui.h>
#include <imgui_impl_dx11.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Image.h"
#include "Saffron/Rendering/Bindables/Texture.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Se
{
class Ui : public Singleton<Ui>
{
public:
	Ui();
	~Ui() override;

	void BeginFrame();
	void EndFrame();

	static void Image(const Texture& texture, const Vector2& size = {0.0f, 0.0f});
	static void Image(const Texture& texture, const Shader& shader, const Vector2& size = {0.0f, 0.0f});
	static void Image(const class Image& image, const Vector2& size = { 0.0f, 0.0f });
	static void Image(const class Image& image, const Shader& shader, const Vector2& size = {0.0f, 0.0f});

private:
	std::vector<std::shared_ptr<const Shader>> _pendingShaders;

	// Custom sampler data

};
}
