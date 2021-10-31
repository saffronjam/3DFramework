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

class BindableStore : public Singleton<BindableStore>
{
public:
	BindableStore();

	template <typename T, typename ... Args>
	static std::shared_ptr<T> Add(Args&& ... args)
	{
		std::shared_ptr<T> result;
		Add(result, std::forward<Args>(args)...);
		return result;
	}

	template <typename T, typename ... Args>
	static bool Add(std::shared_ptr<T>& resource, Args&& ... args)
	{
		if constexpr (HasIdentifier<T, Args...>)
		{
			auto& inst = Instance();
			

			auto classCreateIdentifer = T::Identifier(args...);
			std::ostringstream oss;
			oss << typeid(T).name() << '-' << classCreateIdentifer;

			const auto fullIdentifier = oss.str();
			const auto tryFind = Instance()._storage.find(fullIdentifier);
			if (tryFind == inst._storage.end())
			{
				auto newResource = std::make_shared<T>(std::forward<Args>(args)...);
				newResource->Initialize();
				inst._storage.emplace(fullIdentifier, newResource);
				resource = newResource;
				return true;
			}
			resource = std::static_pointer_cast<T>(tryFind->second);
			return false;
		}
		else
		{
			auto newResource = std::make_shared<T>(std::forward<Args>(args)...);
			newResource->Initialize();
			resource = newResource;
			return true;
		}
	}


private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> _storage;
};
}
