#pragma once

#include <filesystem>
#include <vector>

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Core/Window.h"


namespace Se
{
namespace  fs = std::filesystem;
class FileIOManager
{
public:
	struct Filter
	{
		std::string Description;
		std::vector<std::string> Extensions;

		static Filter Empty();

		Filter() = default;
		Filter(std::string description, std::vector<std::string> extensions)
			: Description(std::move(description)),
			Extensions(std::move(extensions))
		{
		}
	};

public:
	static void Init(const Window &window);

	// Implemented per platform
	static fs::path OpenFile(const Filter &filter = Filter::Empty());
	static fs::path SaveFile(const Filter &filter = Filter::Empty());

	static std::vector<fs::directory_entry> GetFiles(const fs::path &directoryPath, const std::string &extension = "");
	static size_t GetFileCount(const fs::path &directoryPath, const std::string &extension = "");

	static size_t Write(const Uint8 *data, size_t size, const fs::path &filepath, bool overwrite = true);
	static size_t Write(const Buffer buffer, const fs::path &filepath, bool overwrite = true);

	static bool FileExists(const fs::path &filepath);

	static bool Copy(const fs::path &source, const fs::path &destination);

private:
	static const Window *m_Window;

};
}

