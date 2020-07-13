#include "VertexShader.h"

VertexShader::VertexShader( Graphics &gfx, const std::wstring &path )
{
	INFOMAN( gfx );

	GFX_THROW_INFO( D3DReadFileToBlob( path.c_str(), &m_pBytecodeBlob ) );
	GFX_THROW_INFO(
		GetDevice( gfx )->CreateVertexShader(
			m_pBytecodeBlob->GetBufferPointer(),
			m_pBytecodeBlob->GetBufferSize(),
			nullptr,
			&m_pVertexShader
		)
	);
}

void VertexShader::BindTo( Graphics &gfx ) noexcept
{
	GetContext( gfx )->VSSetShader( m_pVertexShader.Get(), nullptr, 0u );
}
