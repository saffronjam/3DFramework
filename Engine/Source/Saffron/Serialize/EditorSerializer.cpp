#include "SaffronPCH.h"

#include "Saffron/Serialize/EditorSerializer.h"

namespace Se
{
EditorSerializer::EditorSerializer(Layer& editorLayer) :
	_editorLayer(editorLayer)
{
}

void EditorSerializer::Serialize(const Path& filepath) const
{
}

bool EditorSerializer::Deserialize(const Path& filepath)
{
	return true;
}
}
