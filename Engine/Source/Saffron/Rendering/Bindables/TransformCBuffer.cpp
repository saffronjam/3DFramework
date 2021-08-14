#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/TransformCBuffer.h"

#include "Saffron/Rendering/Renderer.h"

namespace Se
{
TransformCBuffer::TransformCBuffer() :
	TransformCBuffer({Matrix::Identity, Matrix::Identity, Matrix::Identity, Matrix::Identity})
{
}

TransformCBuffer::TransformCBuffer(const Mvp& mvp) :
	ConstantBuffer(mvp, 0)
{
}

void TransformCBuffer::UploadData()
{
	// Since Hlsl in column major
	_data = {
		_data.Model.Transpose(), _data.ModelView.Transpose(), _data.ViewProjection.Transpose(),
		_data.ModelViewProjection.Transpose()
	};

	ConstantBuffer::UploadData();
}

auto TransformCBuffer::Create() -> std::shared_ptr<TransformCBuffer>
{
	return BindableStore::Add<TransformCBuffer>();
}

auto TransformCBuffer::Create(const Mvp& mvp) -> std::shared_ptr<TransformCBuffer>
{
	return BindableStore::Add<TransformCBuffer>(mvp);
}
}
