#pragma once

#include "Saffron/Base.h"

namespace Se
{
class GenUtils
{
public:
	static ArrayList<String> SplitString(const String& input, const String& delim);
	static WideString ToWide(const String& narrow);
	static String ToNarrow(const String& wide);

private:
	template <class Iter>
	static void SplitStringIter(const String& s, const String& delim, Iter out);
};

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
