#include "file_system.h"
#include <cassert>
#include <fstream>
#include <iosfwd>

namespace Zafkiel
{

std::byte *FileSystem::ReadBytes(std::string filename)
{
    std::ifstream stream(filename, std::ios::binary);
    assert(stream);
    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    size_t size = end - stream.tellg();
    if (size == 0)
    {
        return nullptr;
    }
    std::byte *buffer = new std::byte[size];
    stream.read((char *)buffer, size);
    stream.close();
    return buffer;
}

}
