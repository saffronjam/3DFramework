#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"

typedef void(*GLFWerrorfun) (int, const char *);

namespace Se
{
class Engine
{
public:
	Engine() = delete;
	~Engine() = delete;
	Engine(Engine &) = delete;
	Engine &operator()(Engine &) = delete;

	enum Subsystem
	{
		Graphics = 0
	};

	static void Initialize(Subsystem subsystem);
	static void Uninitialize(Subsystem subsystem);

	static void BindErrorCallback(GLFWerrorfun callback);
	static void BindDefaultErrorCallback();
	static void UnbindErrorCallback();
};

}
