#include "Drawable.h"

#include <cassert>
#include <typeinfo>

#include "Bindable.h"
#include "IndexBuffer.h"

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for ( auto& bind : m_binds )
	{
		bind->BindTo(gfx);
	}
	for ( auto& staticBind : GetStaticBinds() )
	{
		staticBind->BindTo(gfx);
	}
	gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	if ( const auto pDynCast = dynamic_cast<IndexBuffer*>(bind.get()) )
	{
		assert("Attempting to add index buffer a second time" && m_pIndexBuffer == nullptr);
		m_pIndexBuffer = pDynCast;
	}
	m_binds.push_back(std::move(bind));
}
