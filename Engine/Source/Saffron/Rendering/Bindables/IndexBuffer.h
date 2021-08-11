#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class IndexBuffer : public Bindable
{
public:
	explicit IndexBuffer(std::vector<uint> indices);
	explicit IndexBuffer(const uint* data, uint count);

	void Bind() const override;
	

	static auto Create(std::vector<uint> indices) -> std::shared_ptr<IndexBuffer>;
	static auto Create(const uint* data, uint count)->std::shared_ptr<IndexBuffer>;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer;

	std::vector<uint> _indices;
};
}
