#pragma once

#include <string>

class FileIO
{
public:
    FileIO() = default;
    explicit FileIO(std::string preloadedData);
    FileIO(FileIO&& other) noexcept;

    virtual bool LoadFromFile(const std::string &path);

protected:
    std::string m_fileContents;
};


