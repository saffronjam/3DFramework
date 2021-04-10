#pragma once

#include <Saffron.h>

namespace Se
{
class BaseLayer : public Layer, public Signaller
{
public:
	BaseLayer();

	void OnAttach(std::shared_ptr<BatchLoader> &loader) override;
	void OnDetach() override;

	void OnPreFrame() override;
	void OnPostFrame() override;

	void OnUpdate() override;
	void OnGuiRender() override;

protected:

	bool _viewSystem = true;
	bool _viewDemo = true;

private:
	bool _wantResize = false;
	Vector2 _resizeTo = Vector2::Zero;
	int _framesWithNoResizeRequest = 0;
};

};
