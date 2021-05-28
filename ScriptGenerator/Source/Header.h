#pragma once

#include "Base.h"
#include "Class.h"

namespace Se
{
class Header
{
public:
	explicit Header(fs::path path);

	auto Name() const -> const std::string&;
	auto Path() const -> const fs::path&;
	auto RootClasses() const -> std::vector<std::shared_ptr<Class>>;

private:
	void Parse(const std::string_view& data);

private:
	fs::path _path;
	std::string _name;
	std::vector<std::shared_ptr<Class>> _rootClasses;
};
}
