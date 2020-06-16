#pragma once

#include <random>

#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsInitialized()
	{
		return !m_staticBinds.empty();
	}
	static void StaticBind( std::unique_ptr<Bindable> bind ) noexcept( !IS_DEBUG )
	{
		assert( "Use AddStaticIndexBuffer to bind index buffer" && typeid( *bind ) != typeid( IndexBuffer ) );
		m_staticBinds.push_back( std::move( bind ) );
	}
	void AddStaticIndexBuffer( std::unique_ptr<IndexBuffer> ibuf ) noexcept( !IS_DEBUG )
	{
		assert( "Attempted to add index buffer two times" && m_pIndexBuffer == nullptr );
		m_pIndexBuffer = ibuf.get();
		m_staticBinds.push_back( std::move( ibuf ) );
	}
	void SetIndexFromStatic() noexcept( !IS_DEBUG )
	{
		assert( "Attempted to add index buffer two times" && m_pIndexBuffer == nullptr );
		for ( const auto &bind : m_staticBinds )
		{
			if ( const auto pDynCast = dynamic_cast<IndexBuffer *>( bind.get() ) )
			{
				m_pIndexBuffer = pDynCast;
				return;
			}
		}
		assert( "Failed to find index buffer in static binds" && m_pIndexBuffer != nullptr );
	}

private:
	const std::vector<std::unique_ptr<Bindable>> &GetStaticBinds() const noexcept override
	{
		return m_staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::m_staticBinds;
