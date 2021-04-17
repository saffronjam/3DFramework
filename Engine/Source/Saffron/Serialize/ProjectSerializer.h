#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Project.h"

namespace Se
{
class ProjectSerializer
{
public:
	explicit ProjectSerializer(Project& project);

	void Serialize(const Filepath& filepath) const;
	bool Deserialize(const Filepath& filepath);

private:
	Project& _project;
};
}
