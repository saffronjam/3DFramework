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

	void SetImageShader(const std::shared_ptr<Shader>& shader);
	void ResetImageShader();

private:
	std::vector<std::shared_ptr<const Shader>> _pendingShaders;

	// Custom sampler data
};
}
