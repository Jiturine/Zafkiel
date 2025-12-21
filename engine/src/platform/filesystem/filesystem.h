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
    Path RelativeTo(const Path &path) const
    {
        return std::filesystem::relative(*this, path);
    }
};

template <>
struct Serialization<Path>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        out.Value(instance.As<Path>().string());
    }

    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        instance.As<Path>() = data.As<std::string>();
    }
};

class FileSystem
{
  public:
    static std::string ReadText(const Path &filePath);
    static ScopedBuffer ReadBytes(const Path &filePath);
    static std::vector<Path> GetFiles(const Path &directory, const Path &extension);
    static void SaveText(const Path &filePath, const std::string &text);
    static void SaveBytes(const Path &filePath, const std::vector<uint8_t> &data);
};
}