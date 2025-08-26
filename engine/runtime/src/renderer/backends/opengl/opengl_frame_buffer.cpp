#include "opengl_frame_buffer.h"
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
static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
{
    bool multisampled = samples > 1;
    if (multisampled)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
}
static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
{
    bool multisampled = samples > 1;
    if (multisampled)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
    }
    else
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
}
static bool IsDepthFormat(FrameBufferTextureFormat format)
{
    switch (format)
    {
        using enum FrameBufferTextureFormat;
    case DEPTH24STENCIL8:
        return true;
    default: break;
    }
    return false;
}
static GLenum FrameBufferTextureFormatToGL(FrameBufferTextureFormat format)
{
    switch (format)
    {
        using enum FrameBufferTextureFormat;
    case RGBA8:
        return GL_RGBA8;
    case RED_INTEGER:
        return GL_RED_INTEGER;
    case DEPTH24STENCIL8:
        return GL_DEPTH24_STENCIL8;
    default: break;
    }
    Log::CoreError("Unknown FrameBufferTextureFormat!");
    return 0;
}
}

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification &spec)
    : specification(spec)
{
    for (auto format : spec.attachments)
    {
        if (!Utils::IsDepthFormat(format.textureFormat))
            colorAttachmentSpecifications.emplace_back(format.textureFormat);
        else
            depthAttachmentSpecification = format.textureFormat;
    }
    Invalidate();
}
void OpenGLFrameBuffer::Invalidate()
{
    if (rendererID)
    {
        glDeleteFramebuffers(1, &rendererID);
        glDeleteTextures(colorAttachments.size(), colorAttachments.data());
        glDeleteTextures(1, &depthAttachment);
        colorAttachments.clear();
        depthAttachment = 0;
    }
    glGenFramebuffers(1, &rendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
    bool multisample = specification.samples > 1;
    // Attachments
    if (colorAttachmentSpecifications.size())
    {
        colorAttachments.resize(colorAttachmentSpecifications.size());
        Utils::CreateTextures(multisample, colorAttachments.data(), colorAttachments.size());
        for (auto i = 0; i < colorAttachments.size(); i++)
        {
            Utils::BindTexture(multisample, colorAttachments[i]);
            switch (colorAttachmentSpecifications[i].textureFormat)
            {
                using enum FrameBufferTextureFormat;
            case RGBA8:
                Utils::AttachColorTexture(colorAttachments[i], specification.samples, GL_RGBA8, GL_RGBA, specification.width, specification.height, i);
                break;
            case RED_INTEGER:
                Utils::AttachColorTexture(colorAttachments[i], specification.samples, GL_R32I, GL_RED_INTEGER, specification.width, specification.height, i);
                break;
            default: break;
            }
        }
    }
    if (depthAttachmentSpecification.textureFormat != FrameBufferTextureFormat::None)
    {
        Utils::CreateTextures(multisample, &depthAttachment, 1);
        Utils::BindTexture(multisample, depthAttachment);
        switch (depthAttachmentSpecification.textureFormat)
        {
            using enum FrameBufferTextureFormat;
        case DEPTH24STENCIL8:
            Utils::AttachDepthTexture(depthAttachment, specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, specification.width, specification.height);
            break;
        default:
            break;
        }
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
    glDeleteTextures(colorAttachments.size(), colorAttachments.data());
    glDeleteTextures(1, &depthAttachment);
}

uint32_t OpenGLFrameBuffer::GetColorAttachmentRendererID(uint32_t index) const
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
    glViewport(0, 0, specification.width, specification.height);
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
    specification.width = width;
    specification.height = height;
    Invalidate();
}
int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
    if (attachmentIndex >= colorAttachments.size())
    {
        Log::CoreError("Attachment index out of range!");
    }
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}
void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
    if (attachmentIndex >= colorAttachments.size())
    {
        Log::CoreError("Attachment index out of range!");
    }
    auto &spec = colorAttachmentSpecifications[attachmentIndex];
    glClearTexImage(colorAttachments[attachmentIndex], 0, Utils::FrameBufferTextureFormatToGL(spec.textureFormat), GL_INT, &value);
}

}