#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/VertexBuffer.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
VertexBuffer::VertexBuffer(const VertexStorage& storage) :
	_vertexCount(storage.Count()),
	_stride(storage.Stride()),
	_offset(0)
{
	SetInitializer(
		[this, storage]
		{
			const auto inst = ShareThisAs<VertexBuffer>();
			Renderer::Submit(
				[inst, storage](const RendererPackage& package)
				{
					D3D11_BUFFER_DESC bd = {};
					bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bd.ByteWidth = storage.ByteSize();
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.StructureByteStride = storage.Stride();
					bd.Usage = D3D11_USAGE_DEFAULT;

					D3D11_SUBRESOURCE_DATA sd = {};
					sd.pSysMem = storage.Data();

					const auto hr = package.Device.CreateBuffer(&bd, &sd, &inst->_nativeBuffer);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

void VertexBuffer::Bind()
{
	const auto inst = ShareThisAs<VertexBuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.IASetVertexBuffers(
				0,
				1,
				inst->_nativeBuffer.GetAddressOf(),
				&inst->_stride,
				&inst->_offset
			);
		}
	);
}

auto VertexBuffer::VertexCount() const -> size_t
{
	return _vertexCount;
}

auto VertexBuffer::Create(const VertexStorage& storage) -> std::shared_ptr<VertexBuffer>
{
	return BindableStore::Add<VertexBuffer>(storage);
}
}
