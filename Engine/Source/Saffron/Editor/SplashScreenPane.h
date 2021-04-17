#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class SplashScreenPane : public Managed
{
public:
	explicit SplashScreenPane(const Shared<BatchLoader>& batchLoader);

	void OnGuiRender();

	const Shared<BatchLoader>& GetBatchLoader() const { return _batchLoader; }

	void Show();
	void Hide();
	bool IsIdle() const;
	bool IsFinished() const;

private:
	Shared<BatchLoader> _batchLoader;
	Shared<Texture2D> _texture;
	bool _hidden = false;
	String _finalizingStatus;

	float _goalProgressView = 0.0f;
	float _currentProgressView = 0.0f;
	float _currentSinTimer = 0.0f;
};
}
