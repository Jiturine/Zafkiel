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

std::string ReadFile(const fs::path filename)
{
    std::ifstream ifs(filename);
    std::string file;
    ifs >> file;
    ifs.close();
    return file;
}

void SaveFile(const fs::path &filename, const std::string &content)
{
    std::ofstream output_impl_stream(filename);
    output_impl_stream << content;
    output_impl_stream.close();
}

struct ParseConfig
{
    std::vector<fs::path> input_files;
    fs::path output_dir;
    fs::path root_dir;
    std::string module_name;
};

void ParseModule(const ParseConfig &config, const std::vector<const char *> &extraArgs);

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        std::cout << "Usage: " << argv[0] << "<module_name> <input_files> <output_dir> <root_dir> <reflection_macro_file>" << std::endl;
        return 1;
    }

    // 反射的项目名
    std::string module_name(argv[1]);

    // 获取要解析的文件
    std::string input_files = ReadFile(argv[2]);
    auto input_file_strs = Utils::Split(input_files, ";");
    std::vector<fs::path> input_file_list;
    for (const auto &file_str : input_file_strs)
    {
        input_file_list.push_back(fs::path(file_str));
    }

    // 获取输出路径
    fs::path output_dir(argv[3]);

    // 获取要反射的项目根路径
    fs::path root_dir(argv[4]);

    // 获取反射宏所在文件路径
    fs::path reflection_macro_file(argv[5]);

    std::string reflection_macro_str = reflection_macro_file.string();

    // 设置额外参数
    std::vector<const char *> extraArgs = {
        "-std=c++20", "-xc++",
        "-ferror-limit=0", "-Wno-everything",
        "-D__REFLECTION_ENABLE__",                // 定义反射宏
        "-include", reflection_macro_str.c_str(), // 添加反射宏文件路径
    };
    ParseConfig config;
    config.input_files = input_file_list;
    config.output_dir = output_dir;
    config.root_dir = root_dir;
    config.module_name = module_name;

    ParseModule(config, extraArgs);

    return 0;
}

void ParseModule(const ParseConfig &config, const std::vector<const char *> &extraArgs)
{
    if (!fs::exists(config.output_dir))
    {
        fs::create_directory(config.output_dir);
    }

    fs::path output_header_file = config.output_dir / std::format("{}ReflGenerate.h", config.module_name);
    fs::path output_impl_file = config.output_dir / std::format("{}ReflGenerate.cpp", config.module_name);

    HeaderMustacheFormat header_format;
    ImplMustacheFormat impl_format;
    header_format.module_name = config.module_name;
    impl_format.module_name = config.module_name;

    std::cout << "Parsing module: " << config.module_name << std::endl;

    for (const auto &file : config.input_files)
    {
        Parser parser(extraArgs);
        try
        {
            Node *root = parser.ParseFile(file);
            if (root->children.empty())
            {
                delete root;
                continue;
            }
            std::cout << "Parsing file: " << file << std::endl;

            fs::path relative_path = fs::relative(file, config.root_dir / "src");
            std::string filename = relative_path.string();
            Utils::Replace(filename, "../", "");
            std::string func_name = Utils::SubStrBefore(filename, '.');
            Utils::Replace(func_name, '\\', '/');
            Utils::Replace(func_name, "./", "");
            Utils::Replace(func_name, '/', '_');

            const auto &code = GenerateCode(filename, func_name, root);
            const auto &final_filename = func_name + "_refl.h";

            impl_format.refl_header_files.push_back(final_filename);
            impl_format.func_calls.push_back(std::format("Register_{}", func_name));
            SaveFile(config.output_dir / final_filename, code);
            delete root;
        }
        catch (const std::exception &e)
        {
            std::cout << "Failed to Parse File: " << file << " - " << e.what() << std::endl;
        }
    }

    const auto &header_code = MustacheManager::Instance().RenderHeaderFile(header_format);
    const auto &impl_code = MustacheManager::Instance().RenderImplFile(impl_format);
    SaveFile(output_header_file, header_code);
    SaveFile(output_impl_file, impl_code);
}