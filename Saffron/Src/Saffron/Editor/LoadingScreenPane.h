#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class LoadingScreenPane
{
public:
	LoadingScreenPane();
	LoadingScreenPane(Shared<Texture2D> texture);

	void OnGuiRender();

	void SetProgress(Uint32 progress);

private:
	Shared<Texture2D> m_Texture;
	String m_InfoText;
	Atomic<Uint32> m_Progess;
};
}

