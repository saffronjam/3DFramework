#pragma once

#include <filesystem>
#include <vector>

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"


namespace Se
{
namespace  fs = std::filesystem;
class FileIOManager
{
private:
public:
	static std::vector<fs::directory_entry> GetFiles(const fs::path &directoryPath, const std::string &extension = "");
	static size_t GetFileCount(const fs::path &directoryPath, const std::string &extension = "");

	static size_t Write(const Uint8 *data, size_t size, const fs::path &filepath, bool overwrite = true);
	static size_t Write(const Buffer buffer, const fs::path &filepath, bool overwrite = true);

	static bool FileExists(const fs::path &filepath);

	static bool Copy(const fs::path &source, const fs::path &destination);


};
}

