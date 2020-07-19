#pragma once

#include <string>

class FileIO
{
public:
    virtual bool LoadFromFile(const std::string &path);

protected:
    std::string m_fileContents;
};


