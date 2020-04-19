#pragma once

#include "DrawableBase.h"

namespace ve
{
class Box : public DrawableBase<Box>
{
public:
	Box( Graphics &gfx, std::mt19937 &rng,
		 std::uniform_real_distribution<float> &adist,
		 std::uniform_real_distribution<float> &ddist,
		 std::uniform_real_distribution<float> &odist,
		 std::uniform_real_distribution<float> &rdist,
		 std::uniform_real_distribution<float> &bdist
	);

	void Update( const Time &dt ) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	float m_r;
	float m_roll = 0.0f;
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;
	float m_theta;
	float m_phi;
	float m_chi;

	float m_droll;
	float m_dpitch;
	float m_dyaw;
	float m_dtheta;
	float m_dphi;
	float m_dchi;

	DirectX::XMFLOAT3X3 m_modelTransform;
};
}
