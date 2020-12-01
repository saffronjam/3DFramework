#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class SceneEnvironment : public MemManaged<SceneEnvironment>
{
public:
	SceneEnvironment(Filepath filepath, const std::shared_ptr<TextureCube> &radianceMap, const std::shared_ptr<TextureCube> &irradianceMap);

	const Filepath &GetFilepath() const { return m_FilePath; }
	const std::shared_ptr<TextureCube> &GetRadianceMap() const { return m_RadianceMap; }
	const std::shared_ptr<TextureCube> &GetIrradianceMap() const { return m_IrradianceMap; }

	float GetIntensity() const { return m_Intensity; }
	void SetIntensity(float intensity) { m_Intensity = intensity; }

	static std::shared_ptr<SceneEnvironment> Load(const Filepath &filepath);

private:
	Filepath m_FilePath;
	std::shared_ptr<TextureCube> m_RadianceMap;
	std::shared_ptr<TextureCube> m_IrradianceMap;
	float m_Intensity;

};
}