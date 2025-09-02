#pragma once
#include "function/render/texture.h"
#include "platform/filesystem/filesystem.h"
#include <glad/glad.h>

namespace Zafkiel
{
class OpenGLTexture2D : public Texture2D
{
  public:
    OpenGLTexture2D(const TextureSpecification &spec, const Buffer &buffer);
    OpenGLTexture2D(const Path &filePath);
    virtual void SetData(const Buffer &buffer) override;
    virtual ~OpenGLTexture2D();
    virtual uint32_t GetWidth() const override { return width; }
    virtual uint32_t GetHeight() const override { return height; }
    virtual uint32_t GetRendererID() const override { return rendererID; }
    virtual void Bind(uint32_t slot = 0) const override;
  private:
    Path path;
    uint32_t width;
    uint32_t height;
    uint32_t rendererID;
    GLenum internalFormat, dataFormat;
};
}