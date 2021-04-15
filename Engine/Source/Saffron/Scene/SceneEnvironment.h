#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class SceneEnvironment : public Managed
{
public:
	SceneEnvironment(Filepath filepath, const Shared<TextureCube> &radianceMap, const Shared<TextureCube> &irradianceMap);

	const Filepath &GetFilepath() const { return m_FilePath; }
	const Shared<TextureCube> &GetRadianceMap() const { return m_RadianceMap; }
	const Shared<TextureCube> &GetIrradianceMap() const { return m_IrradianceMap; }

	float GetIntensity() const { return m_Intensity; }
	void SetIntensity(float intensity) { m_Intensity = intensity; }

	static Shared<SceneEnvironment> Load(Filepath filepath);

private:
	Filepath m_FilePath;
	Shared<TextureCube> m_RadianceMap;
	Shared<TextureCube> m_IrradianceMap;
	float m_Intensity;

	static constexpr const char *SceneEnvsFolder = "SceneEnvs/";

};
}