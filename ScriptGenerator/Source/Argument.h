#pragma once

#include "Base.h"

namespace Se
{
class Method;

enum class ArgumentType
{
	Int,
	Float,
	Double,
	Char
};

class Argument
{
public:
	explicit Argument(std::shared_ptr<Method> parent);

	auto Type() -> ArgumentType;
	auto Name() -> const std::string&;

	static auto Create(std::shared_ptr<Method> parent, const std::string_view& data) -> std::shared_ptr<Argument>;

	static auto ArgumentTypeToString(ArgumentType argumentType) -> std::string;
	static auto ToArgumentType(const std::string& argumentTypeString) -> ArgumentType;

private:
	void Parse(const std::string_view& data);

private:
	std::shared_ptr<Method> _parent;

	ArgumentType _type;
	std::string _name;
};
}
