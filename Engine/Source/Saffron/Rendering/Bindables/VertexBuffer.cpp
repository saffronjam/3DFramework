#include "SaffronPCH.h"


#include "Saffron/Rendering/Bindables/VertexBuffer.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

#undef max
namespace Se
{
VertexBuffer::VertexBuffer(const VertexLayout& layout, uint capacity) :
	_size(0),
	_capacity(capacity),
	_stride(layout.ByteSize())
{
	SetInitializer(
		[=]
		{
			const auto inst = ShareThisAs<VertexBuffer>();
			Renderer::Submit(
				[=](const RendererPackage& package)
				{
					D3D11_BUFFER_DESC bd = {};
					bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bd.ByteWidth = inst->_stride * inst->_capacity;
					bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					bd.Usage = D3D11_USAGE_DYNAMIC;

					const auto hr = package.Device.CreateBuffer(&bd, nullptr, &inst->_nativeBuffer);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

VertexBuffer::VertexBuffer(const VertexStorage& storage) :
	_size(storage.Count()),
	_capacity(storage.Count()),
	_stride(storage.Stride())
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

void VertexBuffer::Bind() const
{
	const auto inst = ShareThisAs<const VertexBuffer>();
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
	return _size;
}

auto VertexBuffer::Create(const VertexLayout& layout, uint reservedVertexCount) -> std::shared_ptr<VertexBuffer>
{
	return BindableStore::Add<VertexBuffer>(layout, reservedVertexCount);
}

auto VertexBuffer::Create(const VertexStorage& storage) -> std::shared_ptr<VertexBuffer>
{
	return BindableStore::Add<VertexBuffer>(storage);
}
}
