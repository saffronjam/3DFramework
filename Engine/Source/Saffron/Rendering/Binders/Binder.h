#pragma once

#include <memory>

namespace Se
{
template <typename Bindable>
class Binder
{
public:
	explicit Binder(const std::shared_ptr<Bindable>& bindable) :
		_bindable(bindable)
	{
		_bindable->Bind();
	}

	~Binder()
	{
		_bindable->Unbind();
	}

private:
	const std::shared_ptr<Bindable>& _bindable;
};

#define ScopedBinderLine2(bindable, line) Binder binder##line(bindable)
#define ScopedBinderLine(bindable, line) ScopedBinderLine2(bindable, line)
#define ScopedBinder(bindable) ScopedBinderLine(bindable, __LINE__)

}
