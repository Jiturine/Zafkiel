#pragma once
#include "nodes.h"

struct MustacheFormat
{
    std::string parsed_filename;
    std::string refl_func_name;
    std::vector<std::string> class_codes;
    std::vector<std::string> enum_codes;
};

struct ClassMustacheFormat
{
    std::string class_name;
    std::string class_register_name;
    struct PropertyMustacheFormat
    {
        std::string property_name;
        std::string property_register_name;
    };
    std::vector<PropertyMustacheFormat> properties;
};

struct EnumMustacheFormat
{
    std::string enum_name;
    std::string enum_register_name;
    struct ItemMustacheFormat
    {
        std::string item_name;
        std::string item_register_name;
    };
    std::vector<ItemMustacheFormat> items;
};

struct ImplMustacheFormat
{
    std::string header_file;
    std::vector<std::string> refl_header_files;
    std::vector<std::string> func_calls;
};

std::string GenerateCode(const std::string &filename, const std::string &parsed_filename, Node *node);

void GenerateCodeRecursive(const std::string &prefix, Node *node, MustacheFormat &format);

std::string GenerateEnumCode(const std::string &prefix, Node *node);

std::string GenerateClassCode(const std::string &prefix, Node *node);