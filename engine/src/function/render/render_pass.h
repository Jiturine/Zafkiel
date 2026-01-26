#pragma once
#include "function/render/pipeline.h"
#include "function/render/image.h"

namespace Zafkiel
{

class Pipeline;

enum class AttachmentType 
{
    None = 0,
    Color,
    Depth,
    Stencil,
    DepthStencil,
    Swapchain
};

struct AttachmentDescription
{
    ImageFormat format;
    ImageLayout initialLayout;
    ImageLayout finalLayout;
    uint32_t samples;
};

struct SubpassAttachmentReference
{
    uint32_t index;
    ImageLayout layout;
};

struct SubpassSpecification
{
    std::vector<SubpassAttachmentReference> attachmentRefs;
    PipelineType type;
};

struct RenderPassSpecification
{
    std::vector<AttachmentDescription> attachments;
    std::vector<SubpassSpecification> subpasses;
};

struct ClearValue
{
    AttachmentType type;
    ImageFormat format;
    union
    {
        float floatValue;
        vec2 vec2Value;
        vec3 vec3Value;
        vec4 vec4Value;
    };
    union
    {
        uint32_t uintValue;
        uvec2 uvec2Value;
        uvec3 uvec3Value;
        uvec4 uvec4Value;
    };
};

struct RenderPassBeginInfo
{
    RenderHandle renderPass;
    RenderHandle frameBuffer;
    std::vector<ClearValue> clearValues;
};

class RenderPassBackend
{
  public:
    virtual ~RenderPassBackend() = default;
};

class RenderPass final
{
  public:
    RenderPass(const RenderPassSpecification &spec, Scope<RenderPassBackend> backend)
        : spec(spec), backend(std::move(backend)) {}

    Borrow<RenderPassBackend> GetBackend() const { return Borrow(backend); }

  private:
    RenderPassSpecification spec;
    Scope<RenderPassBackend> backend;
};
}