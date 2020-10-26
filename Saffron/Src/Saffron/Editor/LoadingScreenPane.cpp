#include "SaffronPCH.h"

#include "Saffron/Editor/LoadingScreenPane.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
LoadingScreenPane::LoadingScreenPane()
	: LoadingScreenPane(nullptr)
{
}

LoadingScreenPane::LoadingScreenPane(Shared<Texture2D> texture)
	: m_Texture(Move(texture)),
	m_Progess(0)
{
}

void LoadingScreenPane::OnGuiRender()
{
	ImGui::Begin("LoadingScreen");

	ImGui::End();
}

void LoadingScreenPane::SetProgress(Uint32 progress)
{
}
}
