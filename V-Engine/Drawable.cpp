#include "Drawable.h"

#include <cassert>
#include <typeinfo>

#include "Bindable.h"
#include "IndexBuffer.h"

void ve::Drawable::Draw( Graphics &gfx ) const noexcept( !IS_DEBUG )
{
	for ( auto &bind : m_binds )
	{
		bind->Bind( gfx );
	}
	gfx.DrawIndexed( m_pIndexBuffer->GetCount() );
}

void ve::Drawable::AddBind( std::unique_ptr<Bindable> bind ) noexcept( !IS_DEBUG )
{
	assert( "Use AddIndexBuffer to bind index buffer" && typeid( *bind ) != typeid( IndexBuffer ) );
	m_binds.push_back( std::move( bind ) );
}

void ve::Drawable::AddIndexBuffer( std::unique_ptr<class IndexBuffer> ibuf ) noexcept
{
	assert( "Attempting to add index buffer a second time" && m_pIndexBuffer == nullptr );
	m_pIndexBuffer = ibuf.get();
	m_binds.push_back( std::move( ibuf ) );
}
