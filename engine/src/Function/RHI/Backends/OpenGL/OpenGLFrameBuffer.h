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

class OpenGLFrameBuffer
{
  public:
    OpenGLFrameBuffer(const RHIRenderPassInfo &renderPassInfo);

    ~OpenGLFrameBuffer();

    bool Matches(const RHIRenderPassInfo &renderPassInfo);
    
    GLuint GetHandle() const { return handle; }

    uint32 GetWidth() const { return width; }

    uint32 GetHeight() const { return height; }

  private:
    std::vector<GLuint> colorAttachments;
    std::optional<GLuint> depthStencilAttachment;

    uint32 width;
    uint32 height;

    GLuint handle;
};

class OpenGLFrameBufferManager
{
  public:
    OpenGLFrameBufferManager() = default;

    OpenGLFrameBuffer *GetOrCreateFramebuffer(const OpenGLRenderTargetInfo &renderTargetInfo, const RHIRenderPassInfo &renderPassInfo);
  
  private:
    using FrameBufferList = std::vector<Scope<OpenGLFrameBuffer>>; // 同个RenderTargetLayout的所有FrameBuffer
    std::unordered_map<uint32, FrameBufferList> frameBuffers;
};

}
