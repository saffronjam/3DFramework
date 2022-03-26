#include "SaffronPCH.h"

#include "Saffron/Common/Utils.h"

namespace Se
{
std::string Utils::ToUpper(const std::string& string)
{
	std::string result;
	for (const auto& character : string)
	{
		result += std::toupper(character);
	}

	return result;
}
}
