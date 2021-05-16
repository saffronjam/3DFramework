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

	void Serialize(const Path& filepath) const;
	bool Deserialize(const Path& filepath);

private:
	Layer& _editorLayer;
};
}
