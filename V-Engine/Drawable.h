#pragma once

#include "VeMath.h"
#include "Graphics.h"
#include "Time.h"

namespace ve
{
class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	virtual ~Drawable() = default;
	Drawable( const Drawable & ) = delete;
	Drawable &operator=( Drawable & ) = delete;

	virtual void Update( const Time &dt ) = 0;
	void Draw( Graphics &gfx ) const noexcept( !IS_DEBUG );
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
protected:
	void AddBind( std::unique_ptr<Bindable> bind ) noexcept( !IS_DEBUG );
	void AddIndexBuffer( std::unique_ptr<class IndexBuffer> ibuf ) noexcept;
private:
	virtual const std::vector<std::unique_ptr<Bindable>> &GetStaticBinds() const noexcept = 0;
protected:
	const class IndexBuffer *m_pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> m_binds;
};
}
