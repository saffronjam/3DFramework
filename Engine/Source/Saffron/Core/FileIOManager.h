#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Core/Window.h"

namespace Se
{
namespace fs = std::filesystem;

class FileIOManager : public SingleTon<FileIOManager>
{
public:
	struct Filter
	{
		String Description;
		List<String> Extensions{};

		static Filter Empty();

		Filter() = default;

		explicit Filter(String description) :
			Description(Move(description))
		{
		}

		Filter(String description, List<String> extensions) :
			Description(Move(description)),
			Extensions(Move(extensions))
		{
		}
	};

public:
	explicit FileIOManager(const Shared<Window>& window);

	// Implemented per platform
	static Path OpenFile(const Filter& filter = Filter::Empty());
	static Path SaveFile(const Filter& filter = Filter::Empty());

	static List<DirEntry> GetFiles(const Path& directoryPath, const String& extension = "");
	static size_t GetFileCount(const Path& directoryPath, const String& extension = "");
	static size_t GetFileSize(const Path& filepath);

	static size_t Write(const uchar* data, size_t size, const Path& filepath, bool overwrite = true);
	static size_t Write(Buffer buffer, const Path& filepath, bool overwrite = true);

	static bool CreateDirectories(const Path& filepath);

	static bool FileExists(const Path& filepath);

	static bool Copy(const Path& source, const Path& destination);

	static size_t Read(const Path& filepath, OStringStream& destination);
	static List<char> ReadBinary(const Path& filepath);

private:
	const Shared<Window>& _window;
};
}
