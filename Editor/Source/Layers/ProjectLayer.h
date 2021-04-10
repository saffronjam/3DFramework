#pragma once

#include <Saffron.h>

#include "Layers/BaseLayer.h"


namespace Se
{
class ProjectLayer : public BaseLayer
{
public:
	void OnAttach(std::shared_ptr<BatchLoader> &loader) override;
	void OnDetach() override;

	void OnUpdate() override;
	void OnGuiRender() override;

private:
	Shared<EditorScene> _scene;
	Shared<EditorCamera> _editorCamera;
	Shared<Texture> _texture;
};
}
