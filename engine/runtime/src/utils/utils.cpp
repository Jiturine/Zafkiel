#include "utils.h"

namespace Zafkiel
{
namespace Utils
{

void Replace(std::string &source_string, std::string sub_string, const std::string new_string)
{
    std::string::size_type pos = 0;
    while ((pos = source_string.find(sub_string)) != std::string::npos)
    {
        source_string.replace(pos, sub_string.length(), new_string);
    }
}

}
}