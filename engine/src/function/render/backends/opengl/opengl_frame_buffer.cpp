#include "opengl_frame_buffer.h"
#include "opengl_texture.h"
#include <glad/glad.h>

namespace Zafkiel
{
static const uint32_t maxFrameBufferSize = 8192;
namespace Utils
{
static GLenum TextureTarget(bool multisampled)
{
    return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}
static void CreateTextures(bool multisampled, uint32_t *outID, uint32_t count)
{
    glCreateTextures(TextureTarget(multisampled), count, outID);
}
static void BindTexture(bool multisampled, uint32_t id)
{
    glBindTexture(TextureTarget(multisampled), id);
}
}

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification &spec)
    : spec(spec)
{
    Invalidate();
}

void OpenGLFrameBuffer::Invalidate()
{
    if (rendererID)
    {
        glDeleteFramebuffers(1, &rendererID);
        colorAttachments.clear();
        depthAttachment = nullptr;
    }
    glGenFramebuffers(1, &rendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

    for (auto format : spec.attachments)
    {
        TextureSpecification textureSpec = {spec.width, spec.height, format};
        if (format == TextureFormat::DEPTH24STENCIL8)
        {
            depthAttachment = MakeRef<OpenGLTexture2D>(textureSpec);
        }
        else
        {
            colorAttachments.push_back(MakeRef<OpenGLTexture2D>(textureSpec));
        }
    }

    for (size_t i = 0; i < colorAttachments.size(); i++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachments[i]->GetRendererID(), 0);
    }
    if (depthAttachment)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment->GetRendererID(), 0);
    }
    if (colorAttachments.size() > 1)
    {
        if (colorAttachments.size() > 4)
        {
            Log::CoreError("Framebuffer only supports 4 attachments!");
        }
        GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        glDrawBuffers(colorAttachments.size(), buffers);
    }
    else if (colorAttachments.empty())
    {
        glDrawBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Log::CoreError("Framebuffer is incomplete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &rendererID);
}

Ref<Texture2D> OpenGLFrameBuffer::GetColorAttachment(uint32_t index) const
{
    if (index >= colorAttachments.size())
    {
        Log::CoreError("Index out of range!");
    };
    return colorAttachments[index];
}

void OpenGLFrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
    glViewport(0, 0, spec.width, spec.height);
}

void OpenGLFrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > maxFrameBufferSize || height > maxFrameBufferSize)
    {
        Log::CoreWarn("Attempted to resize framebuffer to {}, {}", width, height);
        return;
    }
    spec.width = width;
    spec.height = height;
    Invalidate();
}
uint32_t OpenGLFrameBuffer::ReadPixelUInt(uint32_t attachmentIndex, int x, int y)
{
    if (attachmentIndex >= colorAttachments.size())
    {
        Log::CoreError("Attachment index out of range!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    GLint currentBuffer;
    glGetIntegerv(GL_READ_BUFFER, &currentBuffer);
    if (currentBuffer != GL_COLOR_ATTACHMENT0 + attachmentIndex)
    {
        Log::CoreError("Failed to set read buffer!");
    }

    uint32_t pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixelData);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        Log::CoreError("OpenGL error during ReadPixels: {}", error);
    }
    return pixelData;
}
void OpenGLFrameBuffer::ClearColorAttachment(uint32_t index, const void *value)
{
    if (index >= colorAttachments.size())
    {
        Log::CoreError("Attachment index out of range!");
    }
    colorAttachments[index]->Clear(value);
}

}