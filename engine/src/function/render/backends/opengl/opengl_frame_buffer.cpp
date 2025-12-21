#include "opengl_frame_buffer.h"
#include "opengl_image.h"
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

OpenGLFrameBufferBackend::OpenGLFrameBufferBackend(const FrameBufferSpecification &spec)
    : width(spec.width), height(spec.height)
{
    for (auto attachment : spec.attachments)
    {
        attachments.push_back(attachment);
    }
    glGenFramebuffers(1, &rendererID);
    Invalidate();
}

void OpenGLFrameBufferBackend::Invalidate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

    uint32_t colorAttachmentIndex = 0;
    std::vector<GLenum> drawBuffers;

    for (auto attachment : attachments)
    {
        if (attachment->GetFormat() == ImageFormat::DEPTH24STENCIL8)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachment->GetBackend().As<OpenGLImageBackend>()->GetRendererID(), 0);
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_TEXTURE_2D, attachment->GetBackend().As<OpenGLImageBackend>()->GetRendererID(), 0);
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
            colorAttachmentIndex++;
        }
    }
    if (!drawBuffers.empty())
    {
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());
    }
    else
    {
        glDrawBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Log::Error("Framebuffer is incomplete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFrameBufferBackend::~OpenGLFrameBufferBackend()
{
    glDeleteFramebuffers(1, &rendererID);
}

void OpenGLFrameBufferBackend::Resize(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
    Invalidate();
}

Scope<FrameBuffer> OpenGLFrameBufferFactory::Create(const FrameBufferSpecification &spec)
{
    auto backend = CreateScope<OpenGLFrameBufferBackend>(spec);
    return CreateScope<FrameBuffer>(spec, std::move(backend));
}

}
