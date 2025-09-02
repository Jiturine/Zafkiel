#pragma once
#include <filesystem>

namespace Zafkiel
{

using Path = std::filesystem::path;

class FileSystem
{
  public:
    static std::string ReadText(const Path &filePath);
    static std::byte *ReadBytes(const Path &filePath);
};
}