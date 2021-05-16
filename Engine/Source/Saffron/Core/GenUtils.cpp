#include "SaffronPCH.h"

#include "Saffron/Core/GenUtils.h"

namespace Se
{
List<String> GenUtils::SplitString(const String& input, const String& delim)
{
	List<String> strings;
	SplitStringIter(input, delim, std::back_inserter(strings));
	return strings;
}

WString GenUtils::ToWide(const String& narrow)
{
	return WString(narrow.begin(), narrow.end());
}

String GenUtils::ToNarrow(const String& wide)
{
	return String(wide.begin(), wide.end());
}
}
