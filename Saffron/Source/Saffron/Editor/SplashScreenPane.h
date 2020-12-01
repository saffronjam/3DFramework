#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class SplashScreenPane : public MemManaged<SplashScreenPane>
{
public:
	explicit SplashScreenPane(const std::shared_ptr<BatchLoader> &batchLoader);

	void OnGuiRender();

	const std::shared_ptr<BatchLoader> &GetBatchLoader() const { return m_BatchLoader; }

	void Show();
	void Hide();
	bool IsIdle() const;
	bool IsFinished() const;

private:
	std::shared_ptr<BatchLoader> m_BatchLoader;
	std::shared_ptr<Texture2D> m_Texture;
	bool m_Hidden = false;
	String m_FinalizingStatus;

	float m_GoalProgressView = 0.0f;
	float m_CurrentProgressView = 0.0f;
	float m_CurrentSinTimer = 0.0f;
};
}

