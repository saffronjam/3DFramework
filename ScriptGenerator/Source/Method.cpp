#include "Method.h"

#include "Parse.h"

namespace Se
{
Method::Method(std::shared_ptr<Class> parent) :
	_parent(std::move(parent))
{
}

auto Method::Parent() const -> const std::shared_ptr<Class>&
{
	return _parent;
}

auto Method::ReturnType() const -> const std::string&
{
	return _returnType;
}

auto Method::Name() const -> const std::string&
{
	return _name;
}

auto Method::Args() const -> const std::vector<std::shared_ptr<Argument>>&
{
	return _args;
}

auto Method::Create(std::shared_ptr<Class> parent, const std::string_view& row) -> std::shared_ptr<Method>
{
	auto sharedMethod = std::make_shared<Method>(std::move(parent));

	try
	{
		sharedMethod->Parse(row);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(std::runtime_error(std::string("Failed to parse Method, row: ").append(row)));
	}

	return sharedMethod;
}

void Method::Parse(const std::string_view& row)
{
	const bool trailing = row.find("auto") == 0;

	const auto space = Parse::UntilSpace(row, 0);
	if (trailing)
	{
		const auto arrow = Parse::UntilWord("->", row, 0, true);
		const auto alfaNum = Parse::UntilAlphaNum(row, arrow.TokenIndexIncl + 2, true);
		const auto nonAlfaNum = Parse::UntilNotAlphaNum(row, alfaNum.TokenIndexIncl, false);

		if (nonAlfaNum.IsNpos())
		{
			_returnType = row.substr(alfaNum.TokenIndexIncl);
		}
		else
		{
			_returnType = nonAlfaNum.View;
		}
	}
	else
	{
		_returnType = space.View;
	}

	const auto alfaNum = Parse::UntilAlphaNum(row, space.TokenIndexIncl + 1);
	const auto nonAlfaNum = Parse::UntilNotAlphaNum(row, alfaNum.TokenIndexIncl);
	_name = nonAlfaNum.View;

	const auto openPar = Parse::UntilChar('(', row, nonAlfaNum.TokenIndexIncl, true);
	if (openPar.IsNpos())
	{
		return;
	}

	auto nextArgument = Parse::NextArgument(row, openPar.TokenIndexIncl + 1);
	while (!nextArgument.IsNpos())
	{
		_args.push_back(Argument::Create(shared_from_this(), nextArgument.View));
		nextArgument = Parse::NextArgument(row, nextArgument.TokenIndexIncl + 1);
	}
}
}
