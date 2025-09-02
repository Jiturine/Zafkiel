#include "filesystem.h"
#include <cassert>
#include <fstream>
#include <iosfwd>

namespace Zafkiel
{

std::byte *FileSystem::ReadBytes(const Path &filePath)
{
    std::ifstream stream(filePath, std::ios::binary);
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

std::string FileSystem::ReadText(const Path &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        Log::CoreError("Failed to open file: {}", filePath.c_str());
    }
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

}
