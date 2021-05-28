#include "Argument.h"

#include "Parse.h"

namespace Se
{
Argument::Argument(std::shared_ptr<Method> parent) :
	_parent(parent)
{
}

auto Argument::Type() -> ArgumentType
{
	return _type;
}

auto Argument::Name() -> const std::string&
{
	return _name;
}

auto Argument::Create(std::shared_ptr<Method> parent, const std::string_view& data) -> std::shared_ptr<Argument>
{
	auto sharedArgument = std::make_shared<Argument>(std::move(parent));
	sharedArgument->Parse(data);
	return sharedArgument;
}

auto Argument::ArgumentTypeToString(ArgumentType argumentType) -> std::string
{
#define MATCH_WITH(name, to) if(argumentType == (to)) return (name);

	MATCH_WITH("int", ArgumentType::Int);
	MATCH_WITH("float", ArgumentType::Float);
	MATCH_WITH("double", ArgumentType::Double);
	MATCH_WITH("char", ArgumentType::Char);

	return "Invalid";

#undef MATCH_WITH
}

auto Argument::ToArgumentType(const std::string& argumentTypeString) -> ArgumentType
{
#define MATCH_WITH(name, to) if(argumentTypeString == (name)) return (to);

	MATCH_WITH("int", ArgumentType::Int);
	MATCH_WITH("float", ArgumentType::Float);
	MATCH_WITH("double", ArgumentType::Double);
	MATCH_WITH("char", ArgumentType::Char);

	throw std::runtime_error(std::string("Invalid argument type: ") + argumentTypeString);

#undef MATCH_WITH
}

void Argument::Parse(const std::string_view& data)
{
	const auto spaceCursor = Parse::UntilChar(' ', data, 0, false);
	const auto stringType = spaceCursor.View;
	_type = ToArgumentType(std::string(stringType));
	const auto alfaNum = Parse::UntilAlphaNum(data, spaceCursor.TokenIndexIncl, false);
	const auto notAlfaNum = Parse::UntilNotAlphaNum(data, alfaNum.TokenIndexIncl, false);
	if (notAlfaNum.IsNpos())
	{
		// We reached end, this means there was only the name left 
		_name = data.substr(alfaNum.TokenIndexIncl);
	}
	else
	{
		_name = notAlfaNum.View;
	}
}
}
