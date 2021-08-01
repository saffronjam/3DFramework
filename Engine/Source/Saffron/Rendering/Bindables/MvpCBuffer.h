#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class MvpCBuffer : public Bindable
{
public:
	MvpCBuffer();
	explicit MvpCBuffer(const std::array<Matrix, 3>& mvp);

	void Bind() override;

	void UpdateMatrix(const std::array<Matrix, 3>& mvp);

	static auto Create() -> std::shared_ptr<MvpCBuffer>;
	static auto Create(const std::array<Matrix, 3>& mvp) -> std::shared_ptr<MvpCBuffer>;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer;
	std::array<Matrix, 3> _mvp;
	bool _dirty = false;
};
}
