#include "Header.h"

#include "Parse.h"

namespace Se
{
Header::Header(fs::path path) :
	_path(std::move(path)),
	_name(_path.filename().stem().string())
{
	Parse(FileIoManager::ReadFromFile(_path));
}

auto Header::Name() const -> const std::string&
{
	return _name;
}

auto Header::Path() const -> const fs::path&
{
	return _path;
}

auto Header::RootClasses() const -> std::vector<std::shared_ptr<Class>>
{
	return _rootClasses;
}

static void printSubClasses(const std::shared_ptr<Class>& curr, int depth)
{
	for (int i = 0; i < depth; i++)
	{
		std::cout << '\t';
	}
	std::cout << curr->Name() << ": \n";
	for (const auto& method : curr->Methods())
	{
		for (int i = 0; i < depth; i++)
		{
			std::cout << '\t';
		}
		std::cout << " - " << method->ReturnType() << " " << method->Name() << "(";
		std::string args;
		for (const auto& arg : method->Args())
		{
			args.append(Argument::ArgumentTypeToString(arg->Type())).append(" ").append(arg->Name()).append(", ");
		}
		args = args.substr(0, args.length() - 2);
		std::cout << args;
		std::cout << ")\n";
	}


	for (const auto& sub : curr->SubClasses())
	{
		//std::cout << "\t- ";
		printSubClasses(sub, depth + 1);
	}
}

void Header::Parse(const std::string_view& data)
{
	size_t cIndex = Parse::NextClass(0, data);
	while (cIndex != std::string_view::npos)
	{
		const auto scopeEndIndex = Parse::ScopeEndIndex(cIndex, data.substr(cIndex));
		if (scopeEndIndex != std::string_view::npos)
		{
			auto classType = Class::Create(nullptr, data.substr(cIndex, scopeEndIndex - cIndex + 1));
			_rootClasses.push_back(std::move(classType));

			printSubClasses(_rootClasses.back(), 0);
		}

		cIndex = Parse::NextClass(scopeEndIndex + 1, data.substr(scopeEndIndex + 1));
	}
}
}
