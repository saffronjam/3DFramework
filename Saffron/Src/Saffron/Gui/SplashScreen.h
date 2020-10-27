#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class SplashScreen
{
public:
	explicit SplashScreen();

	void OnGuiRender();

	void Show();
	void Hide();
	bool IsIdle();

private:
	const Unique<BatchLoader> &m_BatchLoader;
	Shared<Texture2D> m_Texture;
	bool m_Hidden = false;

	float m_GoalProgressView = 0.0f;
	float m_CurrentProgressView = 0.0f;
	float m_CurrentSinTimer = 0.0f;
};
}

