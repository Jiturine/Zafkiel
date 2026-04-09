#include "Function/RHI/ShaderCompiler/GlslIncluder.h"
#include "Platform/Filesystem/Filesystem.h"

namespace Zafkiel
{

// 持久化
struct IncludeData
{
	std::string sourceName;
	std::string content;
};

shaderc_include_result* GlslIncluder::GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t)
{
	auto result = new shaderc_include_result;
	Path dstPath(requesting_source);
	Path sourcePath = dstPath.parent_path() / requested_source;

	std::string code = FileSystem::ReadText(rootPath / sourcePath);
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