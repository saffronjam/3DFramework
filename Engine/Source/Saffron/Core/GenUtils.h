#pragma once

#include "Saffron/Base.h"

namespace Se
{
class GenUtils
{
public:
	static List<String> SplitString(const String& input, const String& delim);
	static WString ToWide(const String& narrow);
	static String ToNarrow(const String& wide);

	template <class Class>
	static Pair<String, String> NamespaceAndClassString();

private:
	template <class Iter>
	static void SplitStringIter(const String& s, const String& delim, Iter out);
};

template <class Class>
Pair<String, String> GenUtils::NamespaceAndClassString()
{
	const char* fullname = typeid(Class).name();
	auto result = SplitString(fullname, "::");

	Debug::Assert(!result.empty() || result.size() < 2,
	              "Failed to parse namespace + class string. Should have been <opt. namespace>::<class> but got: {0}",
	              fullname);

	if (result.size() == 1)
	{
		return CreatePair(String(""), result.front());
	}
	if (result.size() == 2)
	{
		return CreatePair(result.front(), result.back());
	}
	return {};
}

template <class Iter>
void GenUtils::SplitStringIter(const String& s, const String& delim, Iter out)
{
	if (delim.empty())
	{
		*out++ = s;
	}
	else
	{
		size_t a = 0, b = s.find(delim);
		for (; b != String::npos; a = b + delim.length(), b = s.find(delim, a))
		{
			*out++ = Move(s.substr(a, b - a));
		}
		*out++ = Move(s.substr(a, s.length() - a));
	}
}
}
