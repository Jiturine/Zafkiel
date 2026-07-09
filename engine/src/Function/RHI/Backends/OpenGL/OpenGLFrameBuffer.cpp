#include "Function/RHI/Backends/OpenGL/OpenGLFrameBuffer.h"
#include "Function/RHI/Backends/OpenGL/OpenGLTexture.h"
#include "Function/RHI/RHIResources.h"
#include "Core/Base/Crc.h"

namespace Zafkiel
{

OpenGLRenderTargetInfo::OpenGLRenderTargetInfo(const RHIRenderPassInfo &renderPassInfo)
{
    OpenGLRenderPassHashStruct hashStruct;

    hashStruct.attachmentCount = renderPassInfo.colorAttachments.size()
                               + (renderPassInfo.depthStencilAttachment.has_value() ? 1 : 0);
    
    for (uint32 i = 0; i < renderPassInfo.colorAttachments.size(); i++)
    {
        colorAttachmentFormats.push_back(renderPassInfo.colorAttachments[i].texture->GetFormat());

        hashStruct.formats[i] = colorAttachmentFormats[i];
    }
    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        depthStencilAttachmentFormat = renderPassInfo.depthStencilAttachment->texture->GetFormat();
        
        uint32 index = renderPassInfo.colorAttachments.size();
        hashStruct.formats[index] = renderPassInfo.depthStencilAttachment.value().texture->GetFormat();
    }

    hash = Crc::MemCrc32(hashStruct);
}

OpenGLFrameBuffer::OpenGLFrameBuffer(const RHIRenderPassInfo &renderPassInfo)
{
    glCreateFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    if (!renderPassInfo.colorAttachments.empty())
    {
        width = renderPassInfo.colorAttachments[0].texture->GetWidth();
        height = renderPassInfo.colorAttachments[0].texture->GetHeight();
    }
    else if (renderPassInfo.depthStencilAttachment.has_value())
    {
        width = renderPassInfo.depthStencilAttachment.value().texture->GetWidth();
        height = renderPassInfo.depthStencilAttachment.value().texture->GetHeight();
    }
    else
    {
        Log::Error("FrameBuffer has no attachments!");
        return;
    }

    uint32 colorAttachmentIndex = 0;
    std::vector<GLenum> drawBuffers;

    for (auto &colotAttachmentInfo : renderPassInfo.colorAttachments)
    {
        auto colorAttachment = static_cast<OpenGLTexture *>(colotAttachmentInfo.texture);
        colorAttachments.push_back(colorAttachment);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_TEXTURE_2D, colorAttachment->GetHandle(), 0);

        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);

        colorAttachmentIndex++;

    }
    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        auto depthStencilAttachment = static_cast<OpenGLTexture *>(renderPassInfo.depthStencilAttachment.value().texture);

        this->depthStencilAttachment = depthStencilAttachment;

        GLenum attachmentType;
        switch (depthStencilAttachment->GetFormat())
        {
            case ImageFormat::DEPTH32F:
                attachmentType = GL_DEPTH_ATTACHMENT;
                break;
            case ImageFormat::DEPTH24STENCIL8:
                attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
                break;
            default:
                attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
                Log::Warn("Unknown depth format, defaulting to GL_DEPTH_STENCIL_ATTACHMENT");
                break;
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, depthStencilAttachment->GetHandle(), 0);
    }

    if (!drawBuffers.empty())
    {
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());
    }
    else
    {
        glDrawBuffer(GL_NONE);
    }

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        const char* errorDesc = nullptr;
        switch (status)
        {
            case GL_FRAMEBUFFER_UNDEFINED: errorDesc = "GL_FRAMEBUFFER_UNDEFINED"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: errorDesc = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: errorDesc = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: errorDesc = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: errorDesc = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: errorDesc = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: errorDesc = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: errorDesc = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
            default: errorDesc = "Unknown framebuffer error"; break;
        }
        Log::Error("Framebuffer is incomplete: {} ({}x{}, color attachments: {}, has depth: {})",
                   errorDesc, width, height, colorAttachments.size(), depthStencilAttachment.has_value());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &handle);
}

bool OpenGLFrameBuffer::Matches(const RHIRenderPassInfo &renderPassInfo)
{
    if (colorAttachments.size() != renderPassInfo.colorAttachments.size())
    {
        return false;
    }

    for (auto [index, colorAttachmentInfo] : std::views::enumerate(renderPassInfo.colorAttachments))
    {
        auto glTexture = static_cast<OpenGLTexture *>(colorAttachmentInfo.texture);

        if (glTexture != colorAttachments[index])
        {
            return false;
        }
    }

    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        auto glTexture = static_cast<OpenGLTexture *>(renderPassInfo.depthStencilAttachment.value().texture);
        if (glTexture != depthStencilAttachment)
        {
            return false;
        }
    }

    return true;
}

bool OpenGLFrameBuffer::ContainTexture(OpenGLTexture *texture) const
{
    for (auto &colorAttachment : colorAttachments)
    {
        if (texture == colorAttachment) return true;
    }
    if (depthStencilAttachment.has_value())
    {
        if (texture == depthStencilAttachment.value()) return true;
    }
    return false;
}

OpenGLFrameBuffer *OpenGLFrameBufferManager::GetOrCreateFramebuffer(const OpenGLRenderTargetInfo &renderTargetInfo, const RHIRenderPassInfo &renderPassInfo)
{
    auto frameBufferHash = renderTargetInfo.hash;

    auto FindFrameBufferInList = [&](FrameBufferList &list)
    {
        OpenGLFrameBuffer *result = nullptr;

        for (auto &frameBuffer : list)
        {
            if (frameBuffer->Matches(renderPassInfo))
            {
                result = frameBuffer.get();
                break;
            }
        }
        return result;
    };
    
    FrameBufferList *frameBufferList;
    if (frameBuffers.contains(frameBufferHash))
    {
        frameBufferList = &frameBuffers[frameBufferHash];
        auto result = FindFrameBufferInList(*frameBufferList);
        if (result)
        {
            return result;
        }
    }
    else 
    {
        frameBuffers[frameBufferHash] = std::vector<Scope<OpenGLFrameBuffer>>();
        frameBufferList = &frameBuffers[frameBufferHash];
    }
    
    auto newFrameBuffer = CreateScope<OpenGLFrameBuffer>(renderPassInfo);
    frameBufferList->push_back(MoveTemp(newFrameBuffer));

    return frameBufferList->back().get();
}

void OpenGLFrameBufferManager::OnDestroyTexture(OpenGLTexture *texture)
{
    for (auto &[hash, frameBufferList] : frameBuffers)
    {
        frameBufferList.erase(
            std::remove_if(frameBufferList.begin(), frameBufferList.end(), [texture](auto &fb) { return fb->ContainTexture(texture); }),
             frameBufferList.end());
    }
}

}
