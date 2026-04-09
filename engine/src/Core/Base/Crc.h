#pragma once
#include "Core/Base/Memory.h"

namespace Zafkiel 
{

class Crc
{
  public:
    static uint32 MemCrc32(Buffer data);

    static uint32 StrCrc32(const std::string &str);

  private:
    static uint32 *GetCrcTable();
};

}