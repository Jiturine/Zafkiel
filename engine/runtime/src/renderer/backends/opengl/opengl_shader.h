#pragma once
#include "../../shader.h"
#include <glad/glad.h>

namespace Zafkiel
{
class OpenGLShader : public Shader
{
  public:
    OpenGLShader(const std::string &filePath);
    virtual ~OpenGLShader();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void Set(const std::string &name, const mat4 &matrix) const override;
    virtual void Set(const std::string &name, const vec4 &vector) const override;
    virtual void Set(const std::string &name, int value) const override;

  private:
    unsigned int rendererID;
};
}