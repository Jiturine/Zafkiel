#include "Platform/Filesystem/Filesystem.h"
#include <cassert>
#include <fstream>

namespace Zafkiel
{

ScopedBuffer FileSystem::ReadBytes(const Path &filePath)
{
    std::ifstream stream(filePath, std::ios::binary);
    if (!stream.is_open())
    {
        Log::Error("Failed to open file: {}", filePath.string().c_str());
        return {};
    }

    stream.seekg(0, std::ios::end);
    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32 size = end - stream.tellg();
    if (size == 0)
    {
        stream.close();
        return {};
    }

    ScopedBuffer buffer(size);
    stream.read(buffer.Data<char>(), size);
    stream.close();
    return MoveTemp(buffer);
}

std::string FileSystem::ReadText(const Path &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        Log::Error("Failed to open file: {}", filePath.string().c_str());
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

void FileSystem::SaveText(const Path &filePath, const std::string &text)
{
    std::filesystem::create_directories(filePath.parent_path());
    std::ofstream file(filePath);
    if (!file)
    {
        Log::Error("Failed to open file: {}", filePath.string());
    }
    file << text;
}

void FileSystem::SaveBytes(const Path &filePath, const std::vector<uint8> &data)
{
    std::filesystem::create_directories(filePath.parent_path());
    std::ofstream file(filePath, std::ios::binary);
    if (!file)
    {
        Log::Error("Failed to open file: {}", filePath.string());
    }
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
}
}
