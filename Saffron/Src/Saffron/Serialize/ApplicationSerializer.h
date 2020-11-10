#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Application.h"

namespace Se
{
class ApplicationSerializer
{
public:
	ApplicationSerializer(Application &application);

	void Serialize(const Filepath &filepath) const;
	bool Deserialize(const Filepath &filepath);

private:
	Application &m_Application;
};
}
