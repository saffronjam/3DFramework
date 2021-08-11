#pragma once

#include <optional>
#include <functional>

#include "Saffron/Common/Managed.h"

namespace Se
{
class Bindable : public virtual Managed<Bindable>
{
public:
	virtual ~Bindable() = default;

	virtual void Bind() const = 0;

	void Initialize();

protected:
	void SetInitializer(std::function<void()> initializer);

private:
	std::optional<std::vector<std::function<void()>>> _initializer;
};
}
