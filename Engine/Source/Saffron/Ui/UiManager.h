#pragma once

#include "Saffron/Base.h"

namespace Se
{
class UiManager : public Singleton<UiManager>
{
public:
	UiManager();
	~UiManager() override;

	void BeginFrame();
	void EndFrame();

	void ResetImageShader();

private:
};
}
