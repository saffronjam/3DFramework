#include "SaffronPCH.h"

#include "Saffron/Renderer/SceneRenderer.h"
#include "Saffron/Scene/SceneEnvironment.h"

namespace Se
{

SceneEnvironment::SceneEnvironment(Filepath filepath, const std::shared_ptr<TextureCube> &radianceMap, const std::shared_ptr<TextureCube> &irradianceMap)
	: m_FilePath(Move(filepath)), m_RadianceMap(radianceMap), m_IrradianceMap(irradianceMap), m_Intensity(1.0f)
{
}

std::shared_ptr<SceneEnvironment> SceneEnvironment::Load(const Filepath &filepath)
{
	std::shared_ptr<SceneEnvironment> newSceneEnvironment = SceneRenderer::CreateSceneEnvironment(filepath);
	return newSceneEnvironment;
}

}
