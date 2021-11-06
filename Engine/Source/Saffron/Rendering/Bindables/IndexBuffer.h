#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class IndexBuffer : public Bindable
{
public:
	explicit IndexBuffer(uint capacity);
	explicit IndexBuffer(const std::vector<uint>& indices);
	explicit IndexBuffer(const uint* data, uint count);

	void Bind() const override;

	void SetIndices(const uint* indices, uint count, uint offset = 0);

	static auto Create(uint capacity)->std::shared_ptr<IndexBuffer>;
	static auto Create(const std::vector<uint>& indices) -> std::shared_ptr<IndexBuffer>;
	static auto Create(const uint* data, uint count) -> std::shared_ptr<IndexBuffer>;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer{};
	
	uint _capacity{};
	uint _size{};
	uint _offset = 0;
};
}
