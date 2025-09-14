#pragma once
#include <filesystem>
#include "core/meta/serializer/custom_serialize.h"

namespace Zafkiel
{

class [[refl]] Path : public std::filesystem::path
{
  public:
    using std::filesystem::path::path;
    Path(std::filesystem::path raw) : std::filesystem::path(raw) {}
    Path RelativeTo(const Path &path)
    {
        return std::filesystem::relative(*this, path);
    }
};

template <>
struct Serialization<Path>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
    {
        out << instance.As<Path>().string();
    }

    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
    {
        instance.As<Path>() = Path(data.as<std::string>());
    }
};

class FileSystem
{
  public:
    static std::string ReadText(const Path &filePath);
    static std::byte *ReadBytes(const Path &filePath);
};
}