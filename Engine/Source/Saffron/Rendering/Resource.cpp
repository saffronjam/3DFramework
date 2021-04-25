#include "SaffronPCH.h"

#include "Saffron/Rendering/Resource.h"

namespace Se
{
ulong Resource::HashFilepath(const Filepath& filepath)
{
	return HashString(filepath.string());
}

ulong Resource::HashString(const String& string)
{
	return std::hash<String>{}(string);
}
}
