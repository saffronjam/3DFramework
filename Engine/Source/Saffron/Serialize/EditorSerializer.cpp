#include "SaffronPCH.h"

#include "Saffron/Serialize/EditorSerializer.h"

namespace Se
{
EditorSerializer::EditorSerializer(Layer& editorLayer) :
	m_EditorLayer(editorLayer)
{
}

void EditorSerializer::Serialize(const Filepath& filepath) const
{
}

bool EditorSerializer::Deserialize(const Filepath& filepath)
{
	return true;
}
}
