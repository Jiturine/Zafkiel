#include "function/render/shader_compiler/glsl_preprocessor.h"

namespace Zafkiel 
{

GlslPreprocessor::GlslPreprocessor(const std::string &source) 
    : source(source)
{
}

ShaderModuleSource GlslPreprocessor::Preprocess()
{
    std::string vertexShaderCode, fragmentShaderCode, geometryShaderCode;
    const char *typeToken = "#type";
    auto typeTokenLength = strlen(typeToken);
    auto pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        auto eol = source.find_first_of("\r\n", pos);
        if (eol == std::string::npos)
        {
            Log::Error("Syntax error");
        }
        auto begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        if (type != "vertex" && type != "fragment" && type != "geometry")
        {
            Log::Error("Invalid shader type");
        }
        auto nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        if (type == "vertex")
            vertexShaderCode = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        else if (type == "fragment")
            fragmentShaderCode = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        else if (type == "geometry")
            geometryShaderCode = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }
    if (vertexShaderCode.empty() || fragmentShaderCode.empty())
    {
        Log::Error("Incompleted shader source!");
    }
    return {vertexShaderCode, fragmentShaderCode, geometryShaderCode};
}


}