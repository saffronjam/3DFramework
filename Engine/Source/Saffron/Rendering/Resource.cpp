#include "SaffronPCH.h"

#include "Saffron/Rendering/Resource.h"

namespace Se
{
bool Resource::IsLoaded() const
{
	return _loaded;
}

void Resource::MarkAsFallback()
{
	_isFallback = true;
}

void Resource::UnmarkAsFallback()
{
	_isFallback = false;
}

bool Resource::IsFallback() const
{
	return _isFallback;
}

void Resource::SuccessfulLoad()
{
	_loaded = true;
}

ulong Resource::HashFilepath(const Path& filepath)
{
	return HashString(filepath.string());
}

ulong Resource::HashString(const String& string)
{
	return std::hash<String>{}(string);
}
}
