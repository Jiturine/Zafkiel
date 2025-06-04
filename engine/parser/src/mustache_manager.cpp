#include "mustache_manager.h"
#include <fstream>
#include <sstream>

MustacheManager::MustacheManager()
{
    class_code_template = mustache::mustache{ReadFile("parser/templates/class_refl.mustache")};
    enum_code_template = mustache::mustache{ReadFile("parser/templates/enum_refl.mustache")};
    code_template = mustache::mustache{ReadFile("parser/templates/refl.mustache")};
    header_template = mustache::mustache{ReadFile("parser/templates/header.mustache")};
    impl_template = mustache::mustache{ReadFile("parser/templates/impl.mustache")};
}

std::string MustacheManager::ReadFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string MustacheManager::RenderHeaderFile()
{
    return header_template.render({});
}

std::string MustacheManager::RenderImplFile(const ImplMustacheFormat &format)
{
    mustache::data data;
    data.set("header_file", format.header_file);

    mustache::data refl_header_files{mustache::data::type::list};
    for (const auto &header_name : format.refl_header_files)
    {
        mustache::data header_file_data;
        header_file_data.set("refl_header_file", header_name);
        refl_header_files << header_file_data;
    }
    mustache::data func_calls{mustache::data::type::list};
    for (const auto &func_name : format.func_calls)
    {
        mustache::data func_data;
        func_data.set("func_call", func_name);
        func_calls << func_data;
    }
    data.set("refl_header_files", refl_header_files);
    data.set("func_calls", func_calls);
    return impl_template.render(data);
}

std::string MustacheManager::RenderClassNode(const ClassMustacheFormat &format)
{
    mustache::data data;
    data.set("class_name", format.class_name);
    data.set("class_register_name", format.class_register_name);
    mustache::data properties{mustache::data::type::list};
    for (const auto &property : format.properties)
    {
        mustache::data property_data;
        property_data.set("property_name", property.property_name);
        property_data.set("property_register_name", property.property_register_name);
        properties << property_data;
    }
    data.set("properties", properties);
    return class_code_template.render(data);
}

std::string MustacheManager::RenderEnumNode(const EnumMustacheFormat &format)
{
    mustache::data data;
    data.set("enum_name", format.enum_name);
    data.set("enum_register_name", format.enum_register_name);
    mustache::data items{mustache::data::type::list};
    for (const auto &item : format.items)
    {
        mustache::data item_data;
        item_data.set("item_name", item.item_name);
        item_data.set("item_register_name", item.item_register_name);
        items << item_data;
    }
    data.set("items", items);
    return enum_code_template.render(data);
}

std::string MustacheManager::RenderCode(const MustacheFormat &format)
{
    mustache::data data;
    data.set("parsed_filename", format.parsed_filename);
    data.set("refl_func_name", format.refl_func_name);
    mustache::data class_code_list{mustache::data::type::list};
    mustache::data enum_code_list{mustache::data::type::list};
    for (const auto &class_code : format.class_codes)
    {
        mustache::data class_data;
        class_data.set("class", class_code);
        class_code_list << class_data;
    }
    for (const auto &enum_code : format.enum_codes)
    {
        mustache::data enum_data;
        enum_data.set("enum", enum_code);
        enum_code_list << enum_data;
    }
    data.set("classes", class_code_list);
    data.set("enums", enum_code_list);
    return code_template.render(data);
}