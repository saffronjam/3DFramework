#include "FileIoManager.h"

#include <filesystem>
#include <vector>
#include <iostream>

namespace Se
{
auto FileIoManager::ReadFromFile(const fs::path& path) -> std::string
{
	std::ifstream in(path);
	char c;

	std::vector<char> data;
	if (in.is_open())
	{
		while (in.good())
		{
			in.get(c);

			if (c != '\n' && c != '\r' && c != '\t')
			{
				data.push_back(c);
			}
		}
	}

	if (!in.eof() && in.fail())
	{
		std::cout << "error reading " << path << std::endl;
	}

	in.close();
	return std::string(data.begin(), data.end());
}
}
