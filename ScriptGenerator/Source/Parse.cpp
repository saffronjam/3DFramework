#include "Parse.h"

namespace Se
{
static constexpr size_t npos = std::string_view::npos;

const ViewCursor ViewCursor::Npos = ViewCursor{"", std::string_view::npos, std::string_view::npos};

ViewCursor::ViewCursor(std::string_view view, std::size_t tokenIndexIncl, std::size_t tokenIndexExcl) :
	View(view),
	TokenIndexIncl(tokenIndexIncl),
	TokenIndexExcl(tokenIndexExcl)
{
}

auto ViewCursor::operator==(const ViewCursor& other) const -> bool
{
	return other.TokenIndexIncl == TokenIndexIncl && other.View == View;
}

auto ViewCursor::IsNpos() const -> bool
{
	return TokenIndexIncl == npos || TokenIndexExcl == npos;
}

auto ViewCursor::ComesBefore(const ViewCursor& other) const -> bool
{
	return Parse::ComesBefore(*this, other);
}

auto Parse::AnyClass(const std::string_view& data) -> bool
{
	for (size_t index = data.find("class"); index < npos; index = data.find("class", index))
	{
		if (IsClassForwardDeclaration(data, index))
		{
			index += std::strlen("class ");
			continue;
		}
		if (IsClassForwardDeclaration(data, index))
		{
			index += std::strlen("class ");
			continue;
		}
		if (IsTemplate(data, index))
		{
			index += std::strlen("class ");
			continue;
		}

		return true;
	}
	return false;
}

auto Parse::IsClassForwardDeclaration(const std::string_view& data, size_t index) -> bool
{
	const auto isForwardDeclaration = index > std::strlen("enum class") && data.find(';', index) <= data.
		find('{', index);
	return isForwardDeclaration;
}

auto Parse::IsEnumClass(const std::string_view& data, size_t index) -> bool
{
	const auto isEnumClass = data.find("enum class", index) <= data.find("class", index);
	return isEnumClass;
}

auto Parse::IsTemplate(const std::string_view& data, size_t index) -> bool
{
	const auto fitsTemplateDecl = index > std::strlen("template<");
	if (fitsTemplateDecl && data.find_first_of(">,", index) < data.find_first_of("{;", index))
	{
		return true;
	}
	return false;
}


auto Parse::NextClass(size_t baseIndex, const std::string_view& data) -> size_t
{
	for (size_t index = data.find("class "); index < npos; index = data.find("class ", index))
	{
		if (IsClassForwardDeclaration(data, index))
		{
			// Add enough to skip this declaration
			index += std::strlen("class ");
			continue;
		}
		if (IsClassForwardDeclaration(data, index))
		{
			index += std::strlen("class ");
			continue;
		}
		if (IsTemplate(data, index))
		{
			index += std::strlen("class ");
			continue;
		}

		return baseIndex + index;
	}
	return npos;
}

auto Parse::NextMethod(const std::string_view& data, size_t startAt) -> ViewCursor
{
	const auto methodDecorator = UntilWord(FnDecorator, data, startAt);

	if (methodDecorator.IsNpos())
	{
		return ViewCursor::Npos;
	}

	const auto decoratorClosed = UntilChar(';', data, methodDecorator.TokenIndexIncl, true);
	if (decoratorClosed.IsNpos())
	{
		return ViewCursor::Npos;
	}

	const auto tryVoid = UntilWord(Void, data, decoratorClosed.TokenIndexIncl + 1, true);
	const auto tryAuto = UntilWord(Auto, data, decoratorClosed.TokenIndexIncl + 1, true);

	if (tryVoid.ComesBefore(tryAuto))
	{
		// Next method is void
		const auto semiColonCursor = UntilSemicolon(data, tryVoid.TokenIndexExcl + 1);
		const auto openBrackCursor = UntilChar('{', data, tryVoid.TokenIndexExcl + 1);
		return semiColonCursor.TokenIndexIncl < openBrackCursor.TokenIndexIncl ? semiColonCursor : openBrackCursor;
	}
	if (tryAuto.ComesBefore(tryVoid))
	{
		const auto semiColonCursor = UntilSemicolon(data, tryAuto.TokenIndexExcl + 1);
		const auto openBrackCursor = UntilChar('{', data, tryAuto.TokenIndexExcl + 1);
		return semiColonCursor.TokenIndexIncl < openBrackCursor.TokenIndexIncl ? semiColonCursor : openBrackCursor;
	}
	return ViewCursor::Npos;
}

auto Parse::NextArgument(const std::string_view& data, size_t startAt) -> ViewCursor
{
	const auto alfaNum = UntilAlphaNum(data, startAt, true);
	const auto spaceComma = UntilWord(" ,", data, alfaNum.TokenIndexIncl, false);
	const auto justComma = UntilChar(',', data, alfaNum.TokenIndexIncl, false);
	const auto endPar = UntilChar(')', data, alfaNum.TokenIndexIncl, false);

	if (spaceComma.ComesBefore(justComma) && spaceComma.ComesBefore(endPar))
	{
		return spaceComma;
	}
	if (justComma.ComesBefore(spaceComma) && justComma.ComesBefore(endPar))
	{
		return justComma;
	}
	if (endPar.ComesBefore(spaceComma) && endPar.ComesBefore(justComma))
	{
		return endPar;
	}
	// Neither was found
	return ViewCursor::Npos;
}

auto Parse::ScopeEndIndex(size_t baseIndex, const std::string_view& data) -> size_t
{
	int counter = 0;
	for (size_t index = data.find('{'); index != npos; index++)
	{
		if (data[index] == '{')
		{
			counter++;
		}
		else if (data[index] == '}')
		{
			counter--;

			// Found starter
			if (counter == 0)
			{
				return baseIndex + index;
			}
		}
	}
	return npos;
}

auto Parse::ComesBefore(const ViewCursor& first, const ViewCursor& second) -> bool
{
	return first.TokenIndexIncl < second.TokenIndexIncl;
}

auto Parse::ComesBefore(size_t first, size_t second) -> bool
{
	return first < second && second != npos;
}

auto Parse::UntilChar(char character, const std::string_view& data, size_t startAt, bool inclusive,
                      int skip) -> ViewCursor
{
	const auto index = data.find(character, startAt);
	if (index == npos)
	{
		return ViewCursor::Npos;
	}

	const auto scopeDepth = ScopeDepth(data, startAt, index);
	if (scopeDepth == 0 && skip == 0)
	{
		// Quick exit if on same scope depth as we started
		const auto end = index - (inclusive ? 0 : 1);
		return {data.substr(startAt, end - startAt + 1), index, index - 1};
	}

	// We need to traverse
	// We can start on index + 1 because we already found one
	const auto pred = [&data](char toFind, char inList)
	{
		return toFind == inList;
	};
	const auto result = UntilManualCharwise(pred, std::string() + character, data, startAt, inclusive, skip);
	return result;
}

auto Parse::UntilAny(const std::string& characters, const std::string_view& data, size_t startAt, bool inclusive,
                     int skip) -> ViewCursor
{
	const auto index = data.find_first_of(characters, startAt);
	if (index == npos)
	{
		return ViewCursor::Npos;
	}

	const auto scopeDepth = ScopeDepth(data, startAt, index);
	if (scopeDepth == 0 && skip == 0)
	{
		// Quick exit if on same scope depth as we started
		const auto end = index - (inclusive ? 0 : 1);
		return {data.substr(startAt, end - startAt + 1), index, index - 1};
	}

	// We need to traverse
	const auto pred = [&data](char toFind, char inList)
	{
		return toFind == inList;
	};
	const auto result = UntilManualCharwise(pred, characters, data, startAt, inclusive, skip);
	return result;
}

auto Parse::UntilCustom(const std::function<bool(char)>& pred, const std::string_view& data, size_t startAt,
                        bool inclusive, int skip) -> ViewCursor
{
	const auto result = UntilManualCustom(pred, data, startAt, inclusive, skip);
	return result;
}

auto Parse::UntilWord(const std::string& word, const std::string_view& data, size_t startAt, bool inclusive,
                      int skip) -> ViewCursor
{
	const auto index = data.find(word, startAt);
	if (index == npos)
	{
		return ViewCursor::Npos;
	}

	const auto scopeDepth = ScopeDepth(data, startAt, index);
	if (scopeDepth == 0 && skip == 0)
	{
		// Quick exit if on same scope depth as we started
		const auto end = index - (inclusive ? -word.length() : 1);
		return {data.substr(startAt, end - startAt), index + word.length() - 1, index - 1};
	}

	// We need to traverse
	const auto result = UntilManualWordwise(word, data, startAt, inclusive, skip);
	return result;
}

auto Parse::UntilSpace(const std::string_view& data, size_t startAt, bool inclusive, int skip) -> ViewCursor
{
	return UntilChar(' ', data, startAt, inclusive, skip);
}

auto Parse::UntilSemicolon(const std::string_view& data, size_t startAt, bool inclusive, int skip) -> ViewCursor
{
	return UntilChar(';', data, startAt, inclusive, skip);
}

auto Parse::UntilAlphaNum(const std::string_view& data, size_t startAt, bool inclusive, int skip) -> ViewCursor
{
	return UntilCustom([](char character) { return std::isalnum(character); }, data, startAt, inclusive, skip);
}

auto Parse::UntilNotAlphaNum(const std::string_view& data, size_t startAt, bool inclusive, int skip) -> ViewCursor
{
	return UntilCustom([](char character) { return !std::isalnum(character); }, data, startAt, inclusive, skip);
}

auto Parse::UntilManualCharwise(const std::function<bool(char, char)>& pred, const std::string& toMatch,
                                const std::string_view& data, size_t startAt, bool inclusive, int skip) -> ViewCursor
{
	int skipCounter = 0;
	int ctxCounter = 0;
	for (size_t searchIndex = startAt; searchIndex < data.length(); searchIndex++)
	{
		if (IsOpening(data[searchIndex]))
		{
			ctxCounter++;
		}
		else if (IsClosing(data[searchIndex]))
		{
			ctxCounter--;

			// We left the context we started in, so any result after here would be invalid
			if (ctxCounter == -1)
			{
				return ViewCursor::Npos;
			}
		}
		if (ctxCounter == 0)
		{
			const auto match = std::any_of(toMatch.begin(), toMatch.end(), [&pred, searchIndex, &data](auto character)
			{
				return pred(character, data[searchIndex]);
			});

			if (match)
			{
				if (skipCounter >= skip)
				{
					const auto end = searchIndex - (inclusive ? 0 : 1);
					return {data.substr(startAt, end - startAt + 1), searchIndex, searchIndex - 1};
				}
				skipCounter++;
			}
		}
	}
	return ViewCursor::Npos;
}

auto Parse::UntilManualWordwise(const std::string& word, const std::string_view& data, size_t startAt, bool inclusive,
                                int skip) -> ViewCursor
{
	int skipCounter = 0;
	int ctxCounter = 0;
	for (size_t searchIndex = startAt; searchIndex < data.length(); searchIndex++)
	{
		if (IsOpening(data[searchIndex]))
		{
			ctxCounter++;
		}
		else if (IsClosing(data[searchIndex]))
		{
			ctxCounter--;

			// We left the context we started in, so any result after here would be invalid
			if (ctxCounter == -1)
			{
				return ViewCursor::Npos;
			}
		}
		if (ctxCounter == 0)
		{
			const auto match = data.find(word, searchIndex) == searchIndex;

			if (match)
			{
				if (skipCounter >= skip)
				{
					const auto end = searchIndex - (inclusive ? -word.length() : 1);
					return {data.substr(startAt, end - startAt), searchIndex + word.length() - 1, searchIndex - 1};
				}
				skipCounter++;
			}
		}
	}
	return ViewCursor::Npos;
}

auto Parse::UntilManualCustom(const std::function<bool(char)>& pred, const std::string_view& data, size_t startAt,
                              bool inclusive, int skip) -> ViewCursor
{
	int skipCounter = 0;
	int ctxCounter = 0;
	for (size_t searchIndex = startAt; searchIndex < data.length(); searchIndex++)
	{
		// Declare before to allow "NotAlphaNum" to work on "Openers" and "Closers"
		const auto match = pred(data[searchIndex]);

		if (!match && IsOpening(data[searchIndex]))
		{
			ctxCounter++;
		}
		else if (!match && IsClosing(data[searchIndex]))
		{
			ctxCounter--;

			// We left the context we started in, so any result after here would be invalid
			if (ctxCounter == -1)
			{
				return ViewCursor::Npos;
			}
		}
		if (ctxCounter == 0)
		{
			if (match)
			{
				if (skipCounter >= skip)
				{
					const auto end = searchIndex - (inclusive ? 0 : 1);
					return {data.substr(startAt, end - startAt + 1), searchIndex, searchIndex - 1};
				}
				skipCounter++;
			}
		}
	}
	return ViewCursor::Npos;
}

auto Parse::IsOpening(char character) -> bool
{
	return std::string(Openers).find(character) != std::string::npos;
}

auto Parse::IsClosing(char character) -> bool
{
	return std::string(Closers).find(character) != std::string::npos;
}

auto Parse::ScopeDepth(const std::string_view& data, size_t from, size_t to) -> int
{
	int ctxCounter = 0;
	for (size_t index = from; index < to; index++)
	{
		// Prevents this from matching arrow notation '->' as closing
		const auto prevChar = data[index > 0 ? index - 1 : index];

		if (IsOpening(data[index]))
		{
			ctxCounter++;
		}
		else if (prevChar != '-' && IsClosing(data[index]))
		{
			ctxCounter--;
		}
	}
	return ctxCounter;
}
}
