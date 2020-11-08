#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Engine
{
	friend class EngineSerializer;

public:
	static const Filepath &GetStartUpSceneFilepath() { return m_StartUpSceneFilepath; }

private:
	static Filepath m_StartUpSceneFilepath;

};
}
