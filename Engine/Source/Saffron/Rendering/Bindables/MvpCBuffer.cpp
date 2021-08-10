#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/MvpCBuffer.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
MvpCBuffer::MvpCBuffer() :
	MvpCBuffer({Matrix::Identity, Matrix::Identity, Matrix::Identity})
{
}

MvpCBuffer::MvpCBuffer(const Mvp& mvp) :
	ConstantBuffer(mvp, 0)
{
}

void MvpCBuffer::UploadData()
{
	// Since Hlsl in column major
	_data = {_data.Model.Transpose(), _data.ViewProjection.Transpose(), _data.ModelViewProjection.Transpose()};

	ConstantBuffer::UploadData();
}

auto MvpCBuffer::Create() -> std::shared_ptr<MvpCBuffer>
{
	return BindableStore::Add<MvpCBuffer>();
}

auto MvpCBuffer::Create(const Mvp& mvp) -> std::shared_ptr<MvpCBuffer>
{
	return BindableStore::Add<MvpCBuffer>(mvp);
}
}
