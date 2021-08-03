#pragma once

#include <imgui.h>

#include "Saffron/Base.h"

namespace Se
{
class Ui : public Singleton<Ui>
{
public:
	Ui();
	~Ui() override;

	void BeginFrame();
	void EndFrame();

private:
};
}
