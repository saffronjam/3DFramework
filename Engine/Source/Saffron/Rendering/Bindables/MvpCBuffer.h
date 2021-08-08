#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
struct Mvp
{
	Matrix Model;
	Matrix ViewProjection;
	Matrix ModelViewProjection;
};

class MvpCBuffer : public Bindable
{
public:
	MvpCBuffer();
	explicit MvpCBuffer(const Mvp& mvp);

	void Bind() override;

	void UpdateTransform(const Mvp& mvp);
	void UploadTransform();
	
	static auto Create() -> std::shared_ptr<MvpCBuffer>;
	static auto Create(const Mvp& mvp) -> std::shared_ptr<MvpCBuffer>;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer;
	Mvp _mvp;
	bool _dirty = false;
};
}
