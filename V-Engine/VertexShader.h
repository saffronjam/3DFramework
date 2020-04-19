#pragma once

#include "Bindable.h"

namespace ve
{
class VertexShader : public Bindable
{
public:
	VertexShader( Graphics &gfx, const std::wstring &path );
	void Bind( Graphics &gfx ) noexcept override;

	ID3DBlob *GetBytecode() const noexcept { return m_pBytecodeBlob.Get(); }
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBytecodeBlob;
};
}

