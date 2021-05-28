#pragma once

#include "Base.h"
#include "Method.h"

namespace Se
{
class Class : public std::enable_shared_from_this<Class>
{
public:
	explicit Class(std::shared_ptr<Class> parent);
	explicit Class(std::string name);

	auto Parent() const -> const std::shared_ptr<Class>&;
	auto Name() const -> const std::string&;
	auto SubClasses() const -> const std::vector<std::shared_ptr<Class>>&;
	auto Methods() const -> const std::vector<std::shared_ptr<Method>>&;

	void AddSubClass(std::shared_ptr<Class> subClass);

	static auto Create(std::shared_ptr<Class> parent, const std::string_view& data) -> std::shared_ptr<Class>;

private:
	void Parse(const std::string_view& data);
	void ParseName(const std::string_view& data);
	void ParseSubClasses(const std::string_view& data);
	void ParseMethods(const std::string_view& data);

private:
	std::shared_ptr<Class> _parent;
	std::string _name;
	std::vector<std::shared_ptr<Class>> _subClasses;
	std::vector<std::shared_ptr<Method>> _methods;
};
}
