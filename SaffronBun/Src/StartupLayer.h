#pragma once

#include <Saffron.h>

using namespace Se;

class StartupLayer : public Layer
{
public:
	StartupLayer();

	void OnAttach(Shared<BatchLoader> &loader) override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnGuiRender() override;
	void OnEvent(const Event &event) override;

private:
};
