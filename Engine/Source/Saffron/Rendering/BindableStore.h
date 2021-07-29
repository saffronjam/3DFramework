#pragma once

#include <sstream>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
template <typename T, typename ... Args> concept HasIdentifier = requires(T& t, Args&& ... args)
{
	t.Identifier(std::forward<Args>(args)...);
};

class BindableStore : public SingleTon<BindableStore>
{
public:
	BindableStore();

	template <typename T, typename ... Args>
	static std::shared_ptr<T> Add(Args&& ... args)
	{
		if constexpr (HasIdentifier<T, Args...>)
		{
			auto& inst = Instance();

			auto classCreateIdentifer = T::Identifier(std::forward<Args>(args)...);
			std::ostringstream oss;
			oss << typeid(T).name() << '-' << classCreateIdentifer;

			const auto fullIdentifier = oss.str();
			const auto tryFind = Instance()._storage.find(fullIdentifier);
			if (tryFind == inst._storage.end() || tryFind->second.expired())
			{
				auto newResource = std::make_shared<T>(std::forward<Args>(args)...);
				newResource->Initialize();
				inst._storage.emplace(fullIdentifier, newResource);
				return newResource;
			}
			return std::static_pointer_cast<T>(tryFind->second.lock());
		}
		else
		{
			auto newResource = std::make_shared<T>(std::forward<Args>(args)...);
			newResource->Initialize();
			return newResource;
		}
	}


private:
	std::unordered_map<std::string, std::weak_ptr<Bindable>> _storage;
};
}
