#include "InputLayout.h"

ve::InputLayout::InputLayout( Graphics &gfx,
							  const std::vector<D3D11_INPUT_ELEMENT_DESC> &layout,
							  ID3DBlob *pVertexShaderByteCode )
{
	INFOMAN( gfx );

	GFX_THROW_INFO(
		GetDevice( gfx )->CreateInputLayout(
			layout.data(),
			(UINT)layout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&m_pInputLayout
		)
	);
}

void ve::InputLayout::Bind( Graphics &gfx ) noexcept
{
	GetContext( gfx )->IASetInputLayout( m_pInputLayout.Get() );
}
