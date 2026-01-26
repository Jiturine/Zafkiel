#pragma once

namespace Zafkiel 
{

struct ShaderModuleSource
{
    std::string vertexModuleSource;
    std::string fragmentModuleSource;
    std::string geometryModuleSource;
};

class GlslPreprocessor 
{
  public:
    explicit GlslPreprocessor(const std::string &source);

    ShaderModuleSource Preprocess();

  private:
    std::string source;
};

}