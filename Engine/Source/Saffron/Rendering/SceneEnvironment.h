#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Managed.h"

namespace Se
{
class SceneEnvironment : public Managed<SceneEnvironment>
{
public:
	void OnUi();

	void Reload() const;

	static std::shared_ptr<SceneEnvironment> Create(const std::filesystem::path& path);

private:
};
}
