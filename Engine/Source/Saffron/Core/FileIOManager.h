#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Core/Window.h"

namespace Se
{
namespace fs = std::filesystem;

class FileIOManager : public Instansiated<FileIOManager>
{
public:
	struct Filter
	{
		String Description;
		ArrayList<String> Extensions{};

		static Filter Empty();

		Filter() = default;

		explicit Filter(String description) :
			Description(Move(description))
		{
		}

		Filter(String description, ArrayList<String> extensions) :
			Description(Move(description)),
			Extensions(Move(extensions))
		{
		}
	};

public:
	explicit FileIOManager(const Shared<Window>& window);

	// Implemented per platform
	static Filepath OpenFile(const Filter& filter = Filter::Empty());
	static Filepath SaveFile(const Filter& filter = Filter::Empty());

	static ArrayList<DirectoryEntry> GetFiles(const Filepath& directoryPath, const String& extension = "");
	static size_t GetFileCount(const Filepath& directoryPath, const String& extension = "");
	static size_t GetFileSize(const Filepath& filepath);

	static size_t Write(const Uint8* data, size_t size, const Filepath& filepath, bool overwrite = true);
	static size_t Write(const Buffer buffer, const Filepath& filepath, bool overwrite = true);

	static bool CreateDirectories(const Filepath& filepath);

	static bool FileExists(const Filepath& filepath);

	static bool Copy(const Filepath& source, const Filepath& destination);

	static size_t Read(const Filepath& filepath, OutputStringStream& destination);
	static ArrayList<char> ReadBinary(const Filepath& filepath);

private:
	const Shared<Window>& _window;
};
}
