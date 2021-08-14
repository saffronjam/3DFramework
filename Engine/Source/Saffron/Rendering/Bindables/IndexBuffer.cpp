#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/IndexBuffer.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
IndexBuffer::IndexBuffer(uint capacity) :
	_capacity(capacity),
	_size(0)
{
	SetInitializer(
		[=]
		{
			const auto inst = ShareThisAs<IndexBuffer>();
			Renderer::Submit(
				[=](const RendererPackage& package)
				{
					D3D11_BUFFER_DESC bd = {};
					bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bd.Usage = D3D11_USAGE_DYNAMIC;
					bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
					bd.MiscFlags = 0;
					bd.ByteWidth = sizeof uint * capacity;
					bd.StructureByteStride = sizeof uint;
					
					const auto hr = package.Device.CreateBuffer(&bd, nullptr, &inst->_nativeBuffer);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

IndexBuffer::IndexBuffer(const std::vector<uint>& indices) :
	_capacity(indices.capacity()),
	_size(indices.size())
{
	SetInitializer(
		[=]
		{
			const auto inst = ShareThisAs<IndexBuffer>();
			Renderer::Submit(
				[=](const RendererPackage& package)
				{
					D3D11_BUFFER_DESC bd = {};
					bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.ByteWidth = sizeof uint * indices.size();
					bd.StructureByteStride = sizeof uint;

					D3D11_SUBRESOURCE_DATA sd = {};
					sd.pSysMem = indices.data();

					const auto hr = package.Device.CreateBuffer(&bd, &sd, &inst->_nativeBuffer);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

IndexBuffer::IndexBuffer(const uint* data, uint count) :
	IndexBuffer({data, data + count})
{
}

void IndexBuffer::Bind() const
{
	const auto inst = ShareThisAs<const IndexBuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.IASetIndexBuffer(inst->_nativeBuffer.Get(), DXGI_FORMAT_R32_UINT, inst->_offset);
		}
	);
}

void IndexBuffer::SetIndices(const uint* indices, uint count, uint offset)
{
	Debug::Assert(count + offset <= _capacity, "Index buffer overflow");

	const auto inst = ShareThisAs<IndexBuffer>();
	Renderer::Submit(
		[=](const RendererPackage& package)
		{
			D3D11_MAPPED_SUBRESOURCE mr = {};
			const auto hr = package.Context.Map(inst->_nativeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
			ThrowIfBad(hr);

			std::memcpy(static_cast<uint*>(mr.pData) + offset, indices, sizeof uint * count);

			package.Context.Unmap(inst->_nativeBuffer.Get(), 0);

			inst->_size = count;
		}
	);
}

auto IndexBuffer::Create(uint capacity) -> std::shared_ptr<IndexBuffer>
{
	return BindableStore::Add<IndexBuffer>(capacity);
}

auto IndexBuffer::Create(const std::vector<uint>& indices) -> std::shared_ptr<IndexBuffer>
{
	return BindableStore::Add<IndexBuffer>(indices);
}

auto IndexBuffer::Create(const uint* data, uint count) -> std::shared_ptr<IndexBuffer>
{
	return BindableStore::Add<IndexBuffer>(data, count);
}
}
