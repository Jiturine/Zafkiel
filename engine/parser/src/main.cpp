#include <clang-c/Index.h>
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

void save_file(const fs::path &output_dir, const std::string &filename, const std::string &content)
{
    std::ofstream output_impl_stream(output_dir / filename);
    output_impl_stream << content;
    output_impl_stream.close();
}

int main(int argc, char **argv)
{
    std::ifstream ifs(argv[1]);
    std::string all_input_file;
    ifs >> all_input_file;
    ifs.close();
    std::cout << "all_input_file: " << all_input_file << std::endl;

    auto input_files = Utils::Split(all_input_file, ";");

    fs::path output_dir(argv[2]);

    if (!fs::exists(output_dir))
    {
        fs::create_directory(output_dir);
    }
    std::string output_header_filename = output_dir / "refl_generate.h";
    std::string output_impl_filename = output_dir / "refl_generate.cpp";
    fs::path base_dir = output_dir.parent_path() / "src";
    ImplMustacheFormat format;
    format.header_file = output_header_filename;
    for (const auto &file : input_files)
    {
        Parser parser;
        Node *root = parser.ParseFile(file);
        std::cout << "Parsing file: " << file << std::endl;
        if (root->children.empty())
        {
            delete root;
            continue;
        }
        // PrintAST(file);
        fs::path relative_path = fs::relative(file, base_dir);
        std::string filename = relative_path.string();
        Utils::Replace(filename, "../", "");
        auto func_name = Utils::SubStrBefore(filename, '.');
        Utils::Replace(func_name, '\\', '/');
        Utils::Replace(func_name, "./", "");
        Utils::Replace(func_name, '/', '_');
        const auto &code = GenerateCode(filename, func_name, root);
        const auto &final_filename = func_name + ".h";
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