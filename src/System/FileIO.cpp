#include "FileIO.h"

#include <fstream>
#include <iostream>

FileIO::FileIO(std::string preloadedData)
        : m_fileContents(std::move(preloadedData))
{

}

FileIO::FileIO(FileIO &&other) noexcept
        : m_fileContents(std::move(other.m_fileContents))
{

}

bool FileIO::LoadFromFile(const std::string &path)
{
    std::string line;
    std::ifstream myfile(path);
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            m_fileContents += line + '\n';
        }
        myfile.close();
    }
    else
    {
        return false;
    }
    return true;
}

