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
	void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		if ( const auto pDynCast = dynamic_cast<IndexBuffer*>(bind.get()) )
		{
			assert("Attempted to add index buffer two times" && m_staticIndexBuffer == nullptr);
			m_pIndexBuffer = pDynCast;
			m_staticIndexBuffer = pDynCast;
		}
		m_staticBinds.push_back(std::move(bind));
	}
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempted to add index buffer two times" && m_pIndexBuffer == nullptr);
		assert("Attempted to add a nullptr static index buffer" && m_staticIndexBuffer != nullptr);
		m_pIndexBuffer = m_staticIndexBuffer;
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return m_staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
	static IndexBuffer* m_staticIndexBuffer;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::m_staticBinds;

template<class T>
IndexBuffer* DrawableBase<T>::m_staticIndexBuffer = nullptr;
