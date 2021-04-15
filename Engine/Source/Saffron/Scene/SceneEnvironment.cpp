#include "SaffronPCH.h"

#include "Saffron/Renderer/SceneRenderer.h"
#include "Saffron/Scene/SceneEnvironment.h"

namespace Se
{
SceneEnvironment::SceneEnvironment(Filepath filepath, const Shared<TextureCube>& radianceMap,
                                   const Shared<TextureCube>& irradianceMap) :
	m_FilePath(Move(filepath)),
	m_RadianceMap(radianceMap),
	m_IrradianceMap(irradianceMap),
	m_Intensity(1.0f)
{
}

Shared<SceneEnvironment> SceneEnvironment::Load(Filepath filepath)
{
	const Filepath fullFilepath = SceneEnvsFolder + Move(filepath).string();
	Shared<SceneEnvironment> newSceneEnvironment = SceneRenderer::CreateSceneEnvironment(fullFilepath);
	return newSceneEnvironment;
}
}
