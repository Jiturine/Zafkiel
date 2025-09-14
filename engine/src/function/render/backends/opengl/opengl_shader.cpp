#include "opengl_shader.h"
#include "platform/filesystem/filesystem.h"
#include <glad/glad.h>

namespace Zafkiel
{
static std::pair<std::string, std::string> ParseShader(const std::string &shaderCode)
{
    std::string vertexShaderCode, fragmentShaderCode;
    const char *typeToken = "#type";
    auto typeTokenLength = strlen(typeToken);
    auto pos = shaderCode.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        auto eol = shaderCode.find_first_of("\r\n", pos);
        if (eol == std::string::npos)
        {
            Log::CoreError("Syntax error");
        }
        auto begin = pos + typeTokenLength + 1;
        std::string type = shaderCode.substr(begin, eol - begin);
        if (type != "vertex" && type != "fragment")
        {
            Log::CoreError("Invalid shader type");
        }
        auto nextLinePos = shaderCode.find_first_not_of("\r\n", eol);
        pos = shaderCode.find(typeToken, nextLinePos);
        if (type == "vertex")
            vertexShaderCode = shaderCode.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? shaderCode.size() - 1 : nextLinePos));
        else
            fragmentShaderCode = shaderCode.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? shaderCode.size() - 1 : nextLinePos));
    }
    if (vertexShaderCode.empty() || fragmentShaderCode.empty())
    {
        Log::CoreError("Incompleted shader source!");
    }
    return {vertexShaderCode, fragmentShaderCode};
}

static void CompileVertexShader(uint32_t shader)
{
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        Log::CoreError("Vertex shader compilation failed:\n{}", infoLog);
    }
}

static void CompileFragmentShader(uint32_t shader)
{
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        Log::CoreError("Fragment shader compilation failed:\n{}", infoLog);
    }
}

static void LinkProgram(uint32_t program)
{
    int success;
    char infoLog[512];
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        Log::CoreError("Shader program linking failed:\n{}", infoLog);
    }
}

static uint32_t CreateShader(const std::string &vertexSource, const std::string &fragmentSource)
{
    const char *cStyleVertexSrc = vertexSource.c_str(), *cStyleFragmentSrc = fragmentSource.c_str();
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &cStyleVertexSrc, nullptr);
    CompileVertexShader(vertexShader);
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &cStyleFragmentSrc, nullptr);
    CompileFragmentShader(fragmentShader);
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    LinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

OpenGLShader::OpenGLShader(const Path &filePath)
{
    std::string source = FileSystem::ReadText(filePath);
    auto [vertexSource, fragmentSource] = ParseShader(source);
    rendererID = CreateShader(vertexSource, fragmentSource);
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(rendererID);
}

void OpenGLShader::Bind() const
{
    glUseProgram(rendererID);
}
void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}

void OpenGLShader::Set(const std::string &name, const mat4 &matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(rendererID, name.c_str()), 1, GL_FALSE, matrix.value());
}
void OpenGLShader::Set(const std::string &name, const vec4 &vector) const
{
    glUniform4f(glGetUniformLocation(rendererID, name.c_str()), vector.x, vector.y, vector.z, vector.w);
}
void OpenGLShader::Set(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(rendererID, name.c_str()), value);
}
void OpenGLShader::Set(const std::string &name, int *value, uint32_t count) const
{
    glUniform1iv(glGetUniformLocation(rendererID, name.c_str()), count, value);
}
}