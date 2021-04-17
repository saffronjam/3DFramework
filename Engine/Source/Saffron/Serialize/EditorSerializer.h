#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Layer.h"

namespace Se
{
// NOT IMPLEMENTED! Was created for wrong reason, saved template for later use
class EditorSerializer
{
public:
	EditorSerializer(Layer& editorLayer);

	void Serialize(const Filepath& filepath) const;
	bool Deserialize(const Filepath& filepath);

private:
	Layer& _editorLayer;
};
}
