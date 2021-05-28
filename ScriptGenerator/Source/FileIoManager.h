#pragma once

#include <fstream>

namespace fs = std::filesystem;

namespace Se
{
class FileIoManager
{
public:
	static auto ReadFromFile(const fs::path& path) -> std::string;
};
}
