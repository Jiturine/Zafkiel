#pragma once

namespace Zafkiel::Utils
{

void Replace(std::string &source_string, std::string sub_string, const std::string new_string);

void Replace(std::string &source_string, char taget_char, const char new_char);
}