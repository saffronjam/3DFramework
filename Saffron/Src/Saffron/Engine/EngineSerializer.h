#pragma once

#include "Saffron/Base.h"

namespace Se
{
class EngineSerializer
{
public:
	static void Serialize(const Filepath &filepath);
	static bool Deserialize(const Filepath &filepath);
};
}
