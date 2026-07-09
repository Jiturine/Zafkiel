#pragma once
#include "Function/RHI/RHIResources.h"
#include <glad/glad.h>

namespace Zafkiel
{

struct OpenGLRenderPassHashStruct
{
    OpenGLRenderPassHashStruct()
    {
        memset(this, 0, sizeof(OpenGLRenderPassHashStruct));
    }
    uint8 attachmentCount;
    uint8 sampleCount;
    ImageFormat formats[MaxRenderTargets];
};

struct OpenGLRenderTargetInfo
{
    OpenGLRenderTargetInfo(const RHIRenderPassInfo &renderPassInfo);
  
    uint32 hash;

    std::vector<ImageFormat> colorAttachmentFormats;

    std::optional<ImageFormat> depthStencilAttachmentFormat;
  
    uint32 attachmentCount;

    uint32 sampleCount;
};

class OpenGLTexture;

class OpenGLFrameBuffer
{
  public:
    OpenGLFrameBuffer(const RHIRenderPassInfo &renderPassInfo);

    ~OpenGLFrameBuffer();

    bool Matches(const RHIRenderPassInfo &renderPassInfo);

    bool ContainTexture(OpenGLTexture *texture) const;
    
    GLuint GetHandle() const { return handle; }

    uint32 GetWidth() const { return width; }

    uint32 GetHeight() const { return height; }

    bool IsValid() const;

  private:
    std::vector<OpenGLTexture *> colorAttachments;
    std::optional<OpenGLTexture *> depthStencilAttachment;

    uint32 width;
    uint32 height;

    GLuint handle;
};

class OpenGLFrameBufferManager
{
  public:
    OpenGLFrameBufferManager() = default;

    OpenGLFrameBuffer *GetOrCreateFramebuffer(const OpenGLRenderTargetInfo &renderTargetInfo, const RHIRenderPassInfo &renderPassInfo);

    void OnDestroyTexture(OpenGLTexture *texture);

  private:
    using FrameBufferList = std::vector<Scope<OpenGLFrameBuffer>>; // 同个RenderTargetLayout的所有FrameBuffer
    std::unordered_map<uint32, FrameBufferList> frameBuffers;
};

}
