#include "Function/RHI/ShaderReflection.h"
#include "Core/Base/Crc.h"

namespace Zafkiel
{

uint32 GetSizeOfShaderFundamentalType(ShaderFundamentalType type)
{
    switch (type) 
    {
        using enum ShaderFundamentalType;
    case Float: return sizeof(float);
    case Float2: return sizeof(float) * 2;
    case Float3: return sizeof(float) * 3;
    case Float4: return sizeof(float) * 4;
    case Int: return sizeof(int);
    case Int2: return sizeof(int) * 2;
    case Int3: return sizeof(int) * 3;
    case Int4: return sizeof(int) * 4;
    case UInt: return sizeof(uint32);
    case UInt2: return sizeof(uint32) * 2;
    case UInt3: return sizeof(uint32) * 3;
    case UInt4: return sizeof(uint32) * 4;
    case Bool: return sizeof(bool);
    default:
        Log::Error("Can't determine exact size!");
        return 0;
    }
}

  
}
  