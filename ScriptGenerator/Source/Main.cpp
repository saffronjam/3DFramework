#include <filesystem>
#include <vector>

#include "FileIoManager.h"
#include "Generator.h"

namespace fs = std::filesystem;

void print_exception(const std::exception& e, int level = 0)
{
	if (level != 0)
	{
		std::cerr << std::string(std::max(0, level * 2 - 1), ' ') << " |" << '\n';
	}
	std::cerr << std::string(level * 2, ' ') << "|--> " << e.what() << '\n';
	try
	{
		std::rethrow_if_nested(e);
	}
	catch (const std::exception& exception)
	{
		print_exception(exception, level + 1);
	} catch (...)
	{
	}
}

int main() // NOLINT(modernize-use-trailing-return-type)
{
	const fs::path Root = "../Engine/Source";

	Se::Generator generator;

	try
	{
		generator.Run();
	}
	catch (const std::exception& fe)
	{
		print_exception(fe);
	}
}
