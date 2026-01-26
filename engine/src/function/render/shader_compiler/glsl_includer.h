#pragma once
#include "platform/filesystem/filesystem.h"
#include <shaderc/shaderc.hpp>
#include <libshaderc_util/file_finder.h>

namespace Zafkiel 
{
class GlslIncluder : public shaderc::CompileOptions::IncluderInterface
{
  public:
	GlslIncluder(const Path &rootPath) : rootPath(rootPath) {}
	virtual shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t) override;
	
	virtual void ReleaseInclude(shaderc_include_result* data) override;
	
  private:
	Path rootPath;
};
}