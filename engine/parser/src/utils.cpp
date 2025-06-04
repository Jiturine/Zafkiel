#include "utils.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

static int parse_flag = 0;
namespace Utils
{

std::vector<std::string> Split(std::string input, std::string pat)
{
    std::vector<std::string> ret_list;
    while (true)
    {
        size_t index = input.find(pat);
        std::string sub_list = input.substr(0, index);
        if (!sub_list.empty()) { ret_list.push_back(sub_list); }
        input.erase(0, index + pat.size());
        if (index == -1) { break; }
    }
    return ret_list;
}

std::string GetFileName(std::string path)
{
    if (path.size() < 1) { return std::string(); }
    std::vector<std::string> result = Split(path, "/");
    if (result.size() < 1) { return std::string(); }
    return result[result.size() - 1];
}

std::string GetNameWithoutFirstM(std::string &name)
{
    std::string result = name;
    if (name.size() > 2 && name[0] == 'm' && name[1] == '_') { result = name.substr(2); }
    return result;
}

std::string GetNameWithoutContainer(std::string name)
{

    size_t left = name.find_first_of('<') + 1;
    size_t right = name.find_last_of('>');
    if (left > 0 && right < name.size() && left < right) { return name.substr(left, right - left); }
    else { return nullptr; }
}

std::string GetStringWithoutQuot(std::string input)
{
    size_t left = input.find_first_of('\"') + 1;
    size_t right = input.find_last_of('\"');
    if (left > 0 && right < input.size() && left < right) { return input.substr(left, right - left); }
    else { return input; }
}

std::string Trim(const std::string &context)
{

    std::string ret_string = context;

    ret_string = ret_string.erase(0, ret_string.find_first_not_of(" "));
    ret_string = ret_string.erase(ret_string.find_last_not_of(" ") + 1);

    return ret_string;
}

std::string SubStrBefore(const std::string &source_string, char separator)
{
    size_t pos = source_string.find(separator);
    if (pos != std::string::npos) { return source_string.substr(0, pos); }
    return std::string(source_string);
}
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

std::string ToUpper(std::string &source_string)
{
    transform(source_string.begin(), source_string.end(), source_string.begin(), ::toupper);
    return source_string;
}

std::string Join(std::vector<std::string> context_list, std::string separator)
{
    std::string ret_string;
    if (context_list.size() == 0) { return ret_string; }
    ret_string = context_list[0];
    for (int index = 1; index < context_list.size(); ++index) { ret_string += separator + context_list[index]; }

    return ret_string;
}

std::string Trim(std::string &source_string, const std::string trim_chars)
{
    size_t left_pos = source_string.find_first_not_of(trim_chars);
    if (left_pos == std::string::npos) { source_string = std::string(); }
    else
    {
        size_t right_pos = source_string.find_last_not_of(trim_chars);
        source_string = source_string.substr(left_pos, right_pos - left_pos + 1);
    }
    return source_string;
}

std::string LoadFile(std::string path)
{
    std::ifstream iFile(path);
    std::string line_string;
    std::ostringstream template_stream;
    if (false == iFile.is_open())
    {
        iFile.close();
        return "";
    }
    while (std::getline(iFile, line_string)) { template_stream << line_string << std::endl; }
    iFile.close();

    return template_stream.str();
}

void SaveFile(const std::string &outpu_string, const std::string &output_file)
{
    std::filesystem::path out_path(output_file);

    if (!std::filesystem::exists(out_path.parent_path()))
    {
        std::filesystem::create_directories(out_path.parent_path());
    }
    std::fstream output_file_stream(output_file, std::ios_base::out);

    output_file_stream << outpu_string << std::endl;
    output_file_stream.flush();
    output_file_stream.close();
    return;
}

void ReplaceAll(std::string &resource_str, std::string sub_str, std::string new_str)
{
    std::string::size_type pos = 0;
    while ((pos = resource_str.find(sub_str)) != std::string::npos)
    {
        resource_str = resource_str.replace(pos, sub_str.length(), new_str);
    }
    return;
}

unsigned long FormatPathString(const std::string &path_string, std::string &out_string)
{
    unsigned int ulRet = 0;
    auto local_path_string = path_string;
    std::filesystem::path local_path;

    local_path = local_path_string;
    if (local_path.is_relative())
    {
        local_path_string = std::filesystem::current_path().string() + "/" + local_path_string;
    }

    ReplaceAll(local_path_string, "\\", "/");
    std::vector<std::string> subString = Split(local_path_string, "/");
    std::vector<std::string> out_sub_string;
    for (auto p : subString)
    {
        if (p == "..") { out_sub_string.pop_back(); }
        else if (p != ".") { out_sub_string.push_back(p); }
    }
    for (int i = 0; i < out_sub_string.size() - 1; i++) { out_string.append(out_sub_string[i] + "/"); }
    out_string.append(out_sub_string[out_sub_string.size() - 1]);
    return 0;
}
std::string ConvertNameToUpperCamelCase(const std::string &name, std::string pat)
{
    std::string ret_string;
    auto &&name_spilts = Split(name, pat);
    for (auto &split_string : name_spilts)
    {
        split_string[0] = toupper(split_string[0]);
        ret_string.append(split_string);
    }
    return ret_string;
}
}