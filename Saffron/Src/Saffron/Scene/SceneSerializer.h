#pragma once



#include "Saffron/Scene/Scene.h"

namespace Se
{
class SceneSerializer
{
public:
	SceneSerializer(const Shared<Scene> &scene);

	void Serialize(const String &filepath);
	void SerializeRuntime(const String &filepath);

	bool Deserialize(const String &filepath);
	bool DeserializeRuntime(const String &filepath);
private:
	Shared<Scene> m_Scene;
};
}

