#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables/Shader.h"
#include "Saffron/Rendering/Bindables/Texture.h"

namespace Se
{
class SceneEnvironment : public Managed<SceneEnvironment>
{
public:
	void OnUi();

	void Reload() const;

	auto Texture() const -> const std::shared_ptr<TextureCube>&;

	static std::shared_ptr<SceneEnvironment> Create(const std::filesystem::path& path);

private:
	std::shared_ptr<TextureCube> _cubeMap;
	std::shared_ptr<class Texture> _equiTex;
	std::shared_ptr<Shader> _generationShader;
};
}
