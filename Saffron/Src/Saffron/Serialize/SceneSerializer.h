#pragma once

#include "Saffron/Base.h"
#include "Saffron/Scene/Scene.h"

namespace YAML
{
class Emitter;
}

namespace Se
{
class SceneSerializer
{
public:
	explicit SceneSerializer(Scene &scene);

	void Serialize(const Filepath &filepath)const;
	void SerializeRuntime(const Filepath &filepath)const;

	bool Deserialize(const Filepath &filepath);
	bool DeserializeRuntime(const Filepath &filepath);

private:
	void SerializeEntity(YAML::Emitter &emitter, Entity entity) const;
	void SerializeEnvironment(YAML::Emitter &emitter) const;
	void SerializeEditorCamera(YAML::Emitter &emitter) const;


private:
	Scene &m_Scene;
};
}

