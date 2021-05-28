#pragma once

#include "Base.h"
#include "Argument.h"

namespace Se
{
class Class;

class Method : public std::enable_shared_from_this<Method>
{
public:
	explicit Method(std::shared_ptr<Class> parent);

	auto Parent() const -> const std::shared_ptr<Class>&;
	auto ReturnType() const -> const std::string&;
	auto Name() const -> const std::string&;
	auto Args() const -> const std::vector<std::shared_ptr<Argument>>&;

	static auto Create(std::shared_ptr<Class> parent, const std::string_view& row) -> std::shared_ptr<Method>;

private:
	void Parse(const std::string_view& row);

private:
	std::shared_ptr<Class> _parent;
	std::string _name;
	std::string _returnType;
	std::vector<std::shared_ptr<Argument>> _args;
};
}
