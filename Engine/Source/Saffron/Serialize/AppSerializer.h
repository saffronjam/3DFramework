#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/App.h"

namespace Se
{
class AppSerializer
{
public:
	AppSerializer(App& application);

	void Serialize(const Path& filepath) const;
	bool Deserialize(const Path& filepath);

private:
	App& _application;
};
}
