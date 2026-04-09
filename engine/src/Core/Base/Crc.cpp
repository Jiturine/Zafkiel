#include "Core/Base/Crc.h"

namespace Zafkiel 
{

uint32 Crc::MemCrc32(Buffer data)
{
    uint32 crc = 0xFFFFFFFF;
    
    auto crcTable = GetCrcTable();

    for (size_t i = 0; i < data.Size<uint8>(); i++)
    {
        uint8 index = (crc ^ data.Data<uint8>()[i]) & 0xFF;
        crc = (crc >> 8) ^ crcTable[index];
    }

    return ~crc;
}

uint32 Crc::StrCrc32(const std::string &str)
{
    uint32 crc = 0xFFFFFFFF;

    auto crcTable = GetCrcTable();

    for (size_t i = 0; i < str.length(); i++)
    {
        uint8 index = (crc ^ str[i]) & 0xFF;
        crc = (crc >> 8) ^ crcTable[index];
    }

    return ~crc;
}

uint32 *Crc::GetCrcTable()
{
    static bool hasInit = false;
    static uint32 crcTable[256];
    if (!hasInit)
    {
        hasInit = true;

        for (uint32 i = 0; i < 256; i++)
        {
            uint32 crc = i;

            for (int j = 0; j < 8; j++)
            {
                if (crc & 1)
                    crc = (crc >> 1) ^ 0xEDB88320;
                else
                    crc >>= 1;
            }
            crcTable[i] = crc;
        }
    }
    return crcTable;
}

}