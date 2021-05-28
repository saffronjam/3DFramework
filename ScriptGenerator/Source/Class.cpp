#include "Class.h"

#include "Parse.h"

namespace Se
{
Class::Class(std::shared_ptr<Class> parent) :
	_parent(std::move(parent))
{
}

Class::Class(std::string name) :
	_name(std::move(name))
{
}

auto Class::Parent() const -> const std::shared_ptr<Class>&
{
	return _parent;
}

auto Class::Name() const -> const std::string&
{
	return _name;
}

auto Class::SubClasses() const -> const std::vector<std::shared_ptr<Class>>&
{
	return _subClasses;
}

auto Class::Methods() const -> const std::vector<std::shared_ptr<Method>>&
{
	return _methods;
}

void Class::AddSubClass(std::shared_ptr<Class> subClass)
{
	_subClasses.push_back(std::move(subClass));
}

auto Class::Create(std::shared_ptr<Class> parent, const std::string_view& data) -> std::shared_ptr<Class>
{
	auto sharedClass = std::make_shared<Class>(std::move(parent));
	sharedClass->Parse(data);
	return sharedClass;
}

void Class::Parse(const std::string_view& data)
{
	try
	{
		ParseName(data);
	}
	catch (...)
	{
		const auto msg = std::string("Failed to parse Class name: ").append(data.substr(0, 15)).append("...");
		std::throw_with_nested(std::runtime_error(msg));
	}

	try
	{
		ParseSubClasses(data);
	}
	catch (...)
	{
		const auto msg = std::string("Failed to parse subclasses in Class: ").append(data.substr(0, 15)).append("...");
		std::throw_with_nested(std::runtime_error(msg));
	}

	try
	{
		ParseMethods(data);
	}
	catch (...)
	{
		const auto msg = std::string("Failed to parse methods in Class: ").append(data.substr(0, 15)).append("...");
		std::throw_with_nested(std::runtime_error(msg));
	}
}

void Class::ParseName(const std::string_view& data)
{
	const auto classNamePrefixLength = std::strlen("class ");
	const auto nameStart = data.substr(classNamePrefixLength);
	const auto nameLength = nameStart.find_first_of(" {:");
	_name = nameStart.substr(0, nameLength);
}

void Class::ParseSubClasses(const std::string_view& data)
{
	size_t cIndex = Parse::NextClass(std::strlen("class "), data.substr(std::strlen("class ")));
	while (cIndex != std::string_view::npos)
	{
		const auto scopeEndIndex = Parse::ScopeEndIndex(cIndex, data.substr(cIndex));
		if (scopeEndIndex != std::string_view::npos)
		{
			auto classType = Create(shared_from_this(), data.substr(cIndex, scopeEndIndex - cIndex + 1));
			_subClasses.push_back(std::move(classType));
		}

		cIndex = Parse::NextClass(scopeEndIndex + 1, data.substr(scopeEndIndex + 1));
	}
}

void Class::ParseMethods(const std::string_view& data)
{
	const auto firstOpenBracket = Parse::UntilChar('{', data, 0);
	auto nextMethod = Parse::NextMethod(data, firstOpenBracket.TokenIndexIncl + 1);
	while (!nextMethod.IsNpos())
	{
		_methods.push_back(Method::Create(shared_from_this(), nextMethod.View));
		nextMethod = Parse::NextMethod(data, nextMethod.TokenIndexIncl);
	}
}
}
