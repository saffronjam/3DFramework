#include "FileIO.h"

#include <fstream>
#include <iostream>

bool FileIO::LoadFromFile(const std::string &path)
{
    std::string line;
    std::ifstream myfile(path);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            m_fileContents += line + '\n';
        }
        myfile.close();
    }
}
