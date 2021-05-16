#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"

namespace Se
{
FileIOManager::Filter FileIOManager::Filter::Empty()
{
	return Filter("All Files", {"*.*"});
}

FileIOManager::FileIOManager(const Shared<Window>& window) :
	SingleTon(this),
	_window(window)
{
}

List<DirEntry> FileIOManager::GetFiles(const Path& directoryPath, const String& extension)
{
	List<DirEntry> output;

	try
	{
		std::copy_if(fs::directory_iterator(directoryPath), fs::directory_iterator{}, std::back_inserter(output),
		             [&](const DirEntry& entry)
		             {
			             return entry.path().extension() == extension;
		             });
	}
	catch (fs::filesystem_error& fe)
	{
		Log::CoreWarn("Failed to get files from directory: {} with file extension: {}. What: ", directoryPath.string(),
		             extension, fe.what());
	}

	return output;
}

size_t FileIOManager::GetFileCount(const Path& directoryPath, const String& extension)
{
	// Return early if no extension is given
	if (extension.empty())
	{
		return std::distance(fs::directory_iterator(directoryPath), fs::directory_iterator{});
	}

	try
	{
		return std::count_if(fs::directory_iterator(directoryPath), fs::directory_iterator{},
		                     [&](const DirEntry& entry)
		                     {
			                     return entry.path().extension() == extension;
		                     });
	}
	catch (fs::filesystem_error& fe)
	{
		Log::CoreWarn("Failed to get file count from directory: {} with file extension: {}. What: ",
		             directoryPath.string(), extension, fe.what());
	}
	return 0;
}

size_t FileIOManager::GetFileSize(const Path& filepath)
{
	try
	{
		return file_size(filepath);
	}
	catch (fs::filesystem_error& fe)
	{
		Log::CoreWarn("Failed to get file size from file: {}. What: ", filepath.string(), fe.what());
	}
	return 0ull;
}

size_t FileIOManager::Write(const uchar* data, size_t size, const Path& filepath, bool overwrite)
{
	const bool fileExists = FileExists(filepath);
	if (!fileExists || fileExists && overwrite)
	{
		OStream ofstream;
		ofstream.open(filepath);
		if (ofstream.good())
		{
			const auto start = ofstream.tellp();
			ofstream.write(reinterpret_cast<const char*>(data), size);
			return ofstream.tellp() - start;
		}
		Log::CoreWarn("Failed to open file: " + filepath.string());
	}
	return 0;
}

size_t FileIOManager::Write(const Buffer buffer, const Path& filepath, bool overwrite)
{
	return Write(buffer.Data(), buffer.Size(), filepath, overwrite);
}

bool FileIOManager::CreateDirectories(const Path& filepath)
{
	std::error_code errorCode;
	return create_directories(filepath, errorCode);
}

bool FileIOManager::FileExists(const Path& filepath)
{
	std::error_code errorCode;
	return exists(filepath, errorCode);
}

bool FileIOManager::Copy(const Path& source, const Path& destination)
{
	std::error_code errorCode;
	copy_file(source, destination, errorCode);
	return static_cast<bool>(errorCode);
}

size_t FileIOManager::Read(const Path& filepath, OStringStream& destination)
{
	String data;
	IStream is(filepath);

	size_t fileSize = 0ull;
	if (is.is_open())
	{
		is.seekg(0, std::ios::end);
		fileSize = is.tellg();
		is.seekg(0);

		while (std::getline(is, data))
		{
			destination << data;
		}
		is.close();
	}
	return fileSize;
}

List<char> FileIOManager::ReadBinary(const Path& filepath)
{
	IStream is(filepath, std::ios::binary);

	List<char> data;
	if (is.is_open())
	{
		data.reserve(GetFileSize(filepath));
		while (is)
		{
			char c;
			is.get(c);
			if (is)
			{
				data.push_back(c);
			}
		}
	}
	return data;
}
}
