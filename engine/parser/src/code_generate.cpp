#include "code_generate.h"
#include "utils.h"
#include "mustache_manager.h"
#include <filesystem>

namespace fs = std::filesystem;

std::string GenerateCode(const std::string &filename, const std::string &func_name, Node *node)
{
    MustacheFormat format;
    format.parsed_filename = filename;
    format.refl_func_name = func_name;
    for (auto child : node->children) { GenerateCodeRecursive("", child, format); }
    return MustacheManager::Instance().RenderCode(format);
}

void GenerateCodeRecursive(const std::string &prefix, Node *node, MustacheFormat &format)
{
    if (node->type == Node::Type::Enum)
    {
        auto code = GenerateEnumCode(prefix, node);
        format.enum_codes.push_back(code);
    }
    if (node->type == Node::Type::Class)
    {
        auto code = GenerateClassCode(prefix, node);
        format.class_codes.push_back(code);
    }
    auto new_prefix = prefix + "::" + node->name;
    for (auto child : node->children) { GenerateCodeRecursive(new_prefix, child, format); }
}

std::string GenerateEnumCode(const std::string &prefix, Node *node)
{
    auto enum_name_with_prefix = prefix + "::" + node->name;
    EnumMustacheFormat format;
    format.enum_name = enum_name_with_prefix;
    format.enum_register_name = node->name;
    auto enum_node = dynamic_cast<EnumNode *>(node);
    for (auto item : enum_node->items)
    {
        auto item_name = enum_name_with_prefix + "::" + item;
        auto item_register_name = item;
        format.items.push_back({item_name, item_register_name});
    }
    return MustacheManager::Instance().RenderEnumNode(format);
}

std::string GenerateClassCode(const std::string &prefix, Node *node)
{
    auto class_name_with_prefix = prefix + "::" + node->name;
    ClassMustacheFormat format;
    format.class_name = class_name_with_prefix;
    format.class_register_name = node->name;
    auto class_node = dynamic_cast<ClassNode *>(node);
    for (auto field : class_node->fields)
    {
        if (field->attr.need_reflect)
        {
            auto field_name = class_name_with_prefix + "::" + field->name;
            auto field_register_name = field->name;
            format.properties.push_back({field_name, field_register_name});
        }
    }
    return MustacheManager::Instance().RenderClassNode(format);
}
