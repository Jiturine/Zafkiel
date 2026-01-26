#pragma once
#include "function/render/texture.h"
#include <glad/glad.h>

namespace Zafkiel
{
class OpenGLTexture2DBackend final : public Texture2DBackend
{
  public:
    OpenGLTexture2DBackend(const Texture2DSpecification &spec, uint32_t imageRendererID);

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
