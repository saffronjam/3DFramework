#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/App.h"

namespace Se
{
class AppSerializer
{
public:
	AppSerializer(App& application);

	void Serialize(const Filepath& filepath) const;
	bool Deserialize(const Filepath& filepath);

private:
	App& _application;
};
}
