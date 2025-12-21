#pragma once
#include "pipeline.h"
#include "image.h"

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

class RenderPassBackend
{
  public:
    virtual ~RenderPassBackend() = default;
};

class RenderPass final
{
  public:
    RenderPass(const RenderPassSpecification &spec, Scope<RenderPassBackend> backend)
        : backend(std::move(backend)) {}
    void AddPipeline(const Scope<Pipeline> &pipeline) { pipelines.push_back(pipeline); }
    Observer<RenderPassBackend> GetBackend() { return backend; }
    const Observer<RenderPassBackend> GetBackend() const { return backend; }

  private:
    std::vector<Observer<Pipeline>> pipelines;
    Scope<RenderPassBackend> backend;
};
}