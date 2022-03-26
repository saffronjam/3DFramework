#pragma once

#include "Saffron/Base.h"

namespace Se::Utils
{
auto ToUpper(const std::string& string) -> std::string;

template <typename T>
auto PascalTypeToString()
{
	std::string rawName(typeid(T).name());
	std::string name = rawName.substr(rawName.find("::") + 2);
	for (size_t i = 0; i < name.length(); i++)
	{
		if (std::isupper(name[i]))
		{
			name.insert(i, " ");
		}
		i++;
	}
	return name;
}
}
