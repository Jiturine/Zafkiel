#include "Core/Base/Utils.h"

namespace Zafkiel::Utils
{

void Replace(std::string &source_string, std::string sub_string, const std::string new_string)
{
    std::string::size_type pos = 0;
    while ((pos = source_string.find(sub_string)) != std::string::npos)
    {
        source_string.replace(pos, sub_string.length(), new_string);
    }
}

void Replace(std::string &source_string, char taget_char, const char new_char)
{
    std::replace(source_string.begin(), source_string.end(), taget_char, new_char);
}
}