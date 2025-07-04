#include <clang-c/Index.h>
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "parser.h"
#include "code_generate.h"
#include "utils.h"
#include "mustache_manager.h"

namespace fs = std::filesystem;

void save_file(const fs::path &output_dir, const fs::path &filename, const std::string &content)
{
    std::ofstream output_impl_stream(output_dir / filename);
    output_impl_stream << content;
    output_impl_stream.close();
}

int main(int argc, char **argv)
{
    // 获取所有要解析的文件
    std::ifstream ifs(argv[1]);
    std::string all_input_file;
    ifs >> all_input_file;
    ifs.close();
    auto input_files = Utils::Split(all_input_file, ";");

    // 获取输出路径
    fs::path output_dir(argv[2]);
    if (!fs::exists(output_dir))
    {
        fs::create_directory(output_dir);
    }

    // 获取反射宏所在文件路径
    fs::path reflection_macro_file(argv[3]);
    auto reflection_macro_file_path = reflection_macro_file.string();
    std::cout << "Reflection macro file path: " << reflection_macro_file_path << std::endl;

    // 设置额外参数
    std::vector<const char *> extraArgs = {
        "-std=c++20", "-xc++",
        "-ferror-limit=0", "-Wno-everything",
        "-D__REFLECTION_ENABLE__",                      // 定义反射宏
        "-include", reflection_macro_file_path.c_str(), // 添加反射宏文件路径
    };
    fs::path output_header_filename = output_dir / "refl_generate.h";
    fs::path output_impl_filename = output_dir / "refl_generate.cpp";
    fs::path base_dir = output_dir.parent_path() / "src";
    ImplMustacheFormat format;
    format.header_file = output_header_filename.string();

    // 解析每个头文件
    for (const auto &file : input_files)
    {
        Parser parser(extraArgs);
        Node *root = parser.ParseFile(file);
        if (root->children.empty())
        {
            delete root;
            continue;
        }
        std::cout << "Parsing file: " << file << std::endl;
        fs::path relative_path = fs::relative(file, base_dir);
        std::string filename = relative_path.string();
        Utils::Replace(filename, "../", "");
        auto func_name = Utils::SubStrBefore(filename, '.');
        Utils::Replace(func_name, '\\', '/');
        Utils::Replace(func_name, "./", "");
        Utils::Replace(func_name, '/', '_');
        const auto &code = GenerateCode(filename, func_name, root);
        const auto &final_filename = func_name + ".h";
        std::cout << final_filename << std::endl;
        format.refl_header_files.push_back(final_filename);
        format.func_calls.push_back(std::format("Register_{}_ReflInfo", func_name));
        save_file(output_dir, final_filename, code);
        delete root;
    }
    const auto &header_code = MustacheManager::Instance().RenderHeaderFile();
    const auto &impl_code = MustacheManager::Instance().RenderImplFile(format);
    save_file(output_dir, output_header_filename, header_code);
    save_file(output_dir, output_impl_filename, impl_code);
    return 0;
}