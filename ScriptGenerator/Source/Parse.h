#pragma once

#include <functional>

#include "Base.h"

namespace Se
{
struct ViewCursor
{
	ViewCursor(std::string_view view, std::size_t tokenIndexIncl, std::size_t tokenIndexExcl);
	
	auto operator==(const ViewCursor& other) const -> bool;

	std::string_view View;
	std::size_t TokenIndexIncl{};
	std::size_t TokenIndexExcl{};

	auto IsNpos() const -> bool;
	auto ComesBefore(const ViewCursor& other) const -> bool;

	static const ViewCursor Npos;
};

class Parse
{
public:
	static auto AnyClass(const std::string_view& data) -> bool;

	static auto IsClassForwardDeclaration(const std::string_view& data, size_t index) -> bool;
	static auto IsEnumClass(const std::string_view& data, size_t index) -> bool;
	static auto IsTemplate(const std::string_view& data, size_t index) -> bool;

	static auto NextClass(size_t baseIndex, const std::string_view& data) -> size_t;
	static auto NextMethod(const std::string_view& data, size_t startAt) -> ViewCursor;
	static auto NextArgument(const std::string_view& data, size_t startAt) -> ViewCursor;

	static auto ScopeEndIndex(size_t baseIndex, const std::string_view& data) -> size_t;

	static auto ComesBefore(const ViewCursor& first, const ViewCursor& second) -> bool;
	static auto ComesBefore(size_t first, size_t second) -> bool;

	// Base Untils
	static auto UntilChar(char character, const std::string_view& data, size_t startAt = 0, bool inclusive = false,
	                      int skip = 0) -> ViewCursor;
	static auto UntilAny(const std::string& characters, const std::string_view& data, size_t startAt = 0,
	                     bool inclusive = false, int skip = 0) -> ViewCursor;
	static auto UntilCustom(const std::function<bool(char)>& pred, const std::string_view& data, size_t startAt = 0,
	                        bool inclusive = false, int skip = 0) -> ViewCursor;
	static auto UntilWord(const std::string& word, const std::string_view& data, size_t startAt = 0,
	                      bool inclusive = false, int skip = 0) -> ViewCursor;

	// Wrapper Untils
	static auto UntilSpace(const std::string_view& data, size_t startAt = 0, bool inclusive = false,
	                       int skip = 0) -> ViewCursor;
	static auto UntilSemicolon(const std::string_view& data, size_t startAt = 0, bool inclusive = false,
	                           int skip = 0) -> ViewCursor;
	static auto UntilAlphaNum(const std::string_view& data, size_t startAt = 0, bool inclusive = false,
	                          int skip = 0) -> ViewCursor;
	static auto UntilNotAlphaNum(const std::string_view& data, size_t startAt = 0, bool inclusive = false,
	                             int skip = 0) -> ViewCursor;

public:
	static auto UntilManualCharwise(const std::function<bool(char, char)>& pred, const std::string& toMatch,
	                                const std::string_view& data, size_t startAt = 0, bool inclusive = false,
	                                int skip = 0) -> ViewCursor;
	static auto UntilManualWordwise(const std::string& word, const std::string_view& data, size_t startAt = 0,
	                                bool inclusive = false, int skip = 0) -> ViewCursor;
	static auto UntilManualCustom(const std::function<bool(char)>& pred, const std::string_view& data,
	                              size_t startAt = 0, bool inclusive = false, int skip = 0) -> ViewCursor;

	static auto IsOpening(char character) -> bool;
	static auto IsClosing(char character) -> bool;

	static auto ScopeDepth(const std::string_view& data, size_t from, size_t to) -> int;

private:
	static constexpr const char* Openers = "{([<\"";
	static constexpr const char* Closers = "})]>\"";

	static constexpr const char* FnDecorator = "SE_FUNCTION";

	static constexpr const char* Void = "void";
	static constexpr const char* Auto = "auto";
};
}
