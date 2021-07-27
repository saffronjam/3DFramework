#pragma once

#include <imgui.h>

#include "Saffron/Base.h"

namespace Se
{
class Ui : public SingleTon<Ui>
{
public:
	Ui();
	~Ui() override;

	void Begin();
	void End();

private:
};
}
