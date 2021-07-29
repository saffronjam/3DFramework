#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/VertexStorage.h"

namespace Se
{
class VertexBuffer : public Bindable
{
public:
	explicit VertexBuffer(const VertexStorage& storage);

	void Bind() override;

	auto VertexCount() const -> size_t;
	
	static auto Create(const VertexStorage& storage) -> std::shared_ptr<VertexBuffer>;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer{};
	size_t _vertexCount;
	uint _stride;
	uint _offset;
};
}
