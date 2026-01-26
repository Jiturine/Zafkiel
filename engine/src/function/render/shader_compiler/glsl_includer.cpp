#include "function/render/shader_compiler/glsl_includer.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{

// 持久化
struct IncludeData
{
	std::string sourceName;
	std::string content;
};

static std::string ParseSchema(const std::string &source)
{
	std::istringstream ss(source);
    std::string line;
    std::string glslPart;
	bool glslPartBegin = false;
    while (std::getline(ss, line)) 
    {
		if (glslPartBegin) {
            glslPart += line + "\n";
        }
		if (line.find("#version") != std::string::npos) {
			glslPartBegin = true;
        } 
    }
	return glslPart;
}

shaderc_include_result* GlslIncluder::GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t)
{
	auto result = new shaderc_include_result;
	Path dstPath(requesting_source);
	Path sourcePath = dstPath.parent_path() / requested_source;
	std::string text = FileSystem::ReadText(rootPath / sourcePath);
	std::string code;
	if (Path(sourcePath).extension().string() == ".zss")
	{
		code = ParseSchema(text);
	}
	else
	{
		code = text;
	}

	auto* data = new IncludeData{sourcePath.string(), code};
	result->source_name = data->sourceName.c_str();
	result->source_name_length = data->sourceName.length();
	result->content = data->content.c_str();
	result->content_length = data->content.length();
	result->user_data = data;
	return result;
}

void GlslIncluder::ReleaseInclude(shaderc_include_result* data)
{
	auto* includeData = static_cast<IncludeData*>(data->user_data);
	delete includeData;
	delete data;
}

}