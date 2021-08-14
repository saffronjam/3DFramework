#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/VertexStorage.h"

namespace Se
{
class VertexBuffer : public Bindable
{
public:
	explicit VertexBuffer(const VertexLayout& layout, uint capacity);
	explicit VertexBuffer(const VertexStorage& storage);

	void Bind() const override;

	auto VertexCount() const -> size_t;

	template <typename VertexType>
	void SetVertices(const VertexType* vertices, uint count, uint offset = 0);

	static auto Create(const VertexLayout& layout, uint reservedVertexCount) -> std::shared_ptr<VertexBuffer>;
	static auto Create(const VertexStorage& storage) -> std::shared_ptr<VertexBuffer>;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer{};
	size_t _size;
	size_t _capacity;
	uint _stride;
	uint _offset = 0;
};

template <typename VertexType>
void VertexBuffer::SetVertices(const VertexType* vertices, uint count, uint offset)
{
	Debug::Assert(count + offset <= _capacity, "Vertex buffer overflow");

	const auto inst = ShareThisAs<VertexBuffer>();
	Renderer::Submit(
		[=](const RendererPackage& package)
		{
			D3D11_MAPPED_SUBRESOURCE mr = {};
			const auto hr = package.Context.Map(inst->_nativeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
			ThrowIfBad(hr);

			std::memcpy(mr.pData, vertices, sizeof VertexType * count);

			package.Context.Unmap(inst->_nativeBuffer.Get(), 0);

			inst->_size = count;
		}
	);
}
}
