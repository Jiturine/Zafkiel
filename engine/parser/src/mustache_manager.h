#pragma once

#include <mustache.hpp>
#include "code_generate.h"

namespace mustache = kainjow::mustache;

class MustacheManager
{
  public:
    static MustacheManager &Instance()
    {
        static MustacheManager instance;
        return instance;
    }
    std::string RenderClassNode(const ClassMustacheFormat &format);
    std::string RenderEnumNode(const EnumMustacheFormat &format);
    std::string RenderCode(const MustacheFormat &format);
    std::string RenderImplFile(const ImplMustacheFormat &format);
    std::string RenderHeaderFile();
  private:
    MustacheManager();
    std::string ReadFile(const std::string &path);
    mustache::mustache class_code_template;
    mustache::mustache enum_code_template;
    mustache::mustache code_template;
    mustache::mustache header_template;
    mustache::mustache impl_template;
};