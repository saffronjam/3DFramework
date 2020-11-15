#pragma once

#include "Saffron/Base.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class SceneSerializer
{
public:
	explicit SceneSerializer(Scene &scene);

	void Serialize(const Filepath &filepath);
	void SerializeRuntime(const Filepath &filepath);

	bool Deserialize(const Filepath &filepath);
	bool DeserializeRuntime(const Filepath &filepath);

private:
	Scene &m_Scene;
};
}

