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
	_mvp(mvp)
{
	// Since Hlsl in column major
	Mvp transposed{_mvp.Model.Transpose(), _mvp.ViewProjection.Transpose(), _mvp.ModelViewProjection.Transpose()};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.ByteWidth = sizeof Mvp;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = &transposed;

	const auto hr = Renderer::Device().CreateBuffer(&bd, &sd, &_nativeBuffer);
	ThrowIfBad(hr);
}

void MvpCBuffer::Bind()
{
	if (_dirty)
	{
		UploadTransform();
	}

	Renderer::Context().VSSetConstantBuffers(0, 1, _nativeBuffer.GetAddressOf());
}

void MvpCBuffer::UpdateTransform(const Mvp& mvp)
{
	_mvp = mvp;
	_dirty = true;
}

void MvpCBuffer::UploadTransform()
{
	// Since Hlsl in column major
	Mvp transposed{_mvp.Model.Transpose(), _mvp.ViewProjection.Transpose(), _mvp.ModelViewProjection.Transpose()};

	D3D11_MAPPED_SUBRESOURCE mr = {};
	const auto hr = Renderer::Context().Map(_nativeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	ThrowIfBad(hr);

	std::memcpy(mr.pData, &transposed, sizeof Mvp);

	Renderer::Context().Unmap(_nativeBuffer.Get(), 0);

	_dirty = false;
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
