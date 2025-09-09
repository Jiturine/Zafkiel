#pragma once
#include <filesystem>

namespace Zafkiel
{

using Path = std::filesystem::path;
static void SerializePath(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
{
    out << instance.As<Path>().string();
}

static void DeserializePath(Any &instance, const Type *typeInfo, const YAML::Node &data)
{
    instance.As<Path>() = Path(data.as<std::string>());
}
static CustomSerialize<Path> customSerializePath({SerializePath, DeserializePath});

class FileSystem
{
  public:
    static std::string ReadText(const Path &filePath);
    static std::byte *ReadBytes(const Path &filePath);
};
}