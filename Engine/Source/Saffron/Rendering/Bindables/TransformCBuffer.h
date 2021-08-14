#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"

namespace Se
{
struct alignas(16) Mvp
{
	Matrix Model;
	Matrix ModelView;
	Matrix ViewProjection;
	Matrix ModelViewProjection;
};

class TransformCBuffer : public ConstantBuffer<Mvp>
{
public:
	TransformCBuffer();
	explicit TransformCBuffer(const Mvp& mvp);

	void UploadData() override;

	static auto Create() -> std::shared_ptr<TransformCBuffer>;
	static auto Create(const Mvp& mvp) -> std::shared_ptr<TransformCBuffer>;
};
}
