#include "Generator.h"

namespace Se
{
void Generator::Run()
{
	auto headerPaths = LoadHeaderPaths(RelativeEngineRoot);

	for (const auto& headerPath : headerPaths)
	{
		if (headerPath.filename().stem() == "UUID")
		{
			int x = 10;
		}


		auto className = headerPath.filename().stem().string();
		if (className == "Entity")
		{
			int x = 10;
		}

		Header header(headerPath);
	}
}

auto Generator::LoadHeaderPaths(const fs::path& root) -> std::vector<fs::path>
{
	std::vector<fs::path> headerPaths;
	for (const auto& entry : fs::recursive_directory_iterator(root))
	{
		if (entry.path().extension() == ".h")
		{
			headerPaths.push_back(entry);
		}
	}
	return headerPaths;
}
}
