#pragma once

#include "cursor.h"
#include "nodes.h"

namespace Utils
{

std::vector<std::string> Split(std::string input, std::string pat);

std::string GetFileName(std::string path);

std::string GetNameWithoutFirstM(std::string &name);

std::string GetNameWithoutContainer(std::string name);

std::string GetStringWithoutQuot(std::string input);

void Replace(std::string &source_string, std::string sub_string, const std::string new_string);

void Replace(std::string &source_string, char taget_char, const char new_char);

std::string SubStrBefore(const std::string &source_string, char separator);

std::string ToUpper(std::string &source_string);

std::string Join(std::vector<std::string> context_list, std::string separator);

std::string Trim(std::string &source_string, const std::string trim_chars);

std::string LoadFile(std::string path);

void SaveFile(const std::string &outpu_string, const std::string &output_file);

void ReplaceAll(std::string &resource_str, std::string sub_str, std::string new_str);

unsigned long FormatPathString(const std::string &path_string, std::string &out_string);

std::string ConvertNameToUpperCamelCase(const std::string &name, std::string pat);

}