#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"

namespace Se
{
struct Mvp
{
	Matrix Model;
	Matrix ViewProjection;
	Matrix ModelViewProjection;
};

class MvpCBuffer : public ConstantBuffer<Mvp, false>
{
public:
	MvpCBuffer();
	explicit MvpCBuffer(const Mvp& mvp);

	void UploadData() override;

	static auto Create() -> std::shared_ptr<MvpCBuffer>;
	static auto Create(const Mvp& mvp) -> std::shared_ptr<MvpCBuffer>;
};
}
