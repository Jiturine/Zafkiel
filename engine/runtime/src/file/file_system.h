#pragma once

namespace Zafkiel
{
class FileSystem
{
  private:
    static std::byte *ReadBytes(std::string filename);
};
}