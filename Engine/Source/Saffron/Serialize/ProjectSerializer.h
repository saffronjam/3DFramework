#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Project.h"

namespace Se
{
class ProjectSerializer
{
public:
	explicit ProjectSerializer(Project& project);

	void Serialize(const Path& filepath) const;
	bool Deserialize(const Path& filepath);

private:
	Project& _project;
};
}
