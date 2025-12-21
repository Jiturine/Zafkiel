#pragma once
#include "function/render/texture.h"
#include "platform/filesystem/filesystem.h"
#include <glad/glad.h>

namespace Zafkiel
{
class OpenGLTexture2DBackend final : public Texture2DBackend
{
  public:
    OpenGLTexture2DBackend(const Texture2DSpecification &spec);

    void SetData(Observer<Image> image, Buffer buffer);

};

class OpenGLTexture2DFactory final : public Texture2DFactory<OpenGLTexture2DFactory>
{
  public:
    static Scope<Texture2D> Create(const Texture2DSpecification &spec);
    static Scope<Texture2D> Create(const Texture2DSpecification &spec, Buffer buffer);
};

// class OpenGLCubeMap : public CubeMap
// {
//   public:
//     OpenGLCubeMap(const std::vector<Path> &paths);
//     virtual uint32_t GetFaceSize() const override { return faceSize; }
//   private:
//     uint32_t faceSize;
//     uint32_t rendererID;
// };

}
