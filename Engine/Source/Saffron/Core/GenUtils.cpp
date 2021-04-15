#include "SaffronPCH.h"

#include "Saffron/Core/GenUtils.h"

namespace Se
{
ArrayList<String> GenUtils::SplitString(const String& input, const String& delim)
{
	ArrayList<String> strings;
	SplitStringIter(input, delim, std::back_inserter(strings));
	return strings;
}

WideString GenUtils::ToWide(const String& narrow)
{
	return WideString(narrow.begin(), narrow.end());
}

String GenUtils::ToNarrow(const String& wide)
{
	return String(wide.begin(), wide.end());
}
}
