#include "filesystem.h"
#include "core/base/buffer.h"
#include <cassert>
#include <fstream>
#include <iosfwd>

namespace Zafkiel
{

Buffer FileSystem::ReadBytes(const Path &filePath)
{
    std::ifstream stream(filePath, std::ios::binary);
    assert(stream);
    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    size_t size = end - stream.tellg();
    if (size == 0)
    {
        return Buffer();
    }
    std::byte *buffer = new std::byte[size];
    stream.read((char *)buffer, size);
    stream.close();
    return Buffer(buffer, size);
}

std::string FileSystem::ReadText(const Path &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        Log::CoreError("Failed to open file: {}", filePath.string().c_str());
    }
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

std::vector<Path> FileSystem::GetFiles(const Path &directory, const Path &extension)
{
    std::vector<Path> files;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == extension)
        {
            files.push_back(entry.path());
        }
    }
    return files;
}
}
