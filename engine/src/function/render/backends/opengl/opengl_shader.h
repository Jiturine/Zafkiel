#pragma once
#include "function/render/shader.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{
class OpenGLShader : public Shader
{
  public:
    OpenGLShader(const Path &filePath);
    virtual ~OpenGLShader();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void Set(const std::string &name, const mat4 &matrix) const override;
    virtual void Set(const std::string &name, const vec4 &vector) const override;
    virtual void Set(const std::string &name, int value) const override;
    virtual void Set(const std::string &name, uint32_t value) const override;
    virtual void Set(const std::string &name, int *value, uint32_t count) const override;

    virtual uint32_t GetRendererID() const override { return rendererID; }

  private:
    unsigned int rendererID;
};
}