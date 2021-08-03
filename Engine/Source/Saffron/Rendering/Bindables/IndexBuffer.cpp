#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/IndexBuffer.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
IndexBuffer::IndexBuffer(std::vector<uint> indices) :
	_indices(std::move(indices))
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<IndexBuffer>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					D3D11_BUFFER_DESC bd = {};
					bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.ByteWidth = sizeof decltype(inst->_indices)::value_type * inst->_indices.size();
					bd.StructureByteStride = sizeof decltype(inst->_indices)::value_type;
					
					D3D11_SUBRESOURCE_DATA sd = {};
					sd.pSysMem = inst->_indices.data();

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

void IndexBuffer::Bind()
{
	const auto inst = ShareThisAs<IndexBuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.IASetIndexBuffer(inst->_nativeBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}
	);
}

auto IndexBuffer::Create(std::vector<uint> indices) -> std::shared_ptr<IndexBuffer>
{
	return BindableStore::Add<IndexBuffer>(indices);
}

auto IndexBuffer::Create(const uint* data, uint count) -> std::shared_ptr<IndexBuffer>
{
	return BindableStore::Add<IndexBuffer>(data, count);
}
}
