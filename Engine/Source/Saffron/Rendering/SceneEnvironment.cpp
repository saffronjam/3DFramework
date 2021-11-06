#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneEnvironment.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/Bindables/Texture.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"

namespace Se
{
std::shared_ptr<SceneEnvironment> SceneEnvironment::Create(const std::filesystem::path& path)
{
	const auto envMapSize = Renderer::Config().EnvironmentMapResolution;
	const auto inst = std::make_shared<SceneEnvironment>();

	inst->_equiTex = Texture::Create(path);
	inst->_cubeMap = TextureCube::Create(
		envMapSize,
		envMapSize,
		ImageFormat::RGBA32f,
		{ .Usage = TextureUsage_UnorderedAccess | TextureUsage_ShaderResource }
	);
	inst->_generationShader = Shader::Create("EquirectangularToCubeMap", true);


	inst->Draw();

	return inst;
}
}
