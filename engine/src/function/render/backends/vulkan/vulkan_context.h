#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL.h>
#include "function/render/backends/vulkan/vulkan_texture.h"
#include "function/render/global_render_resource.h"
#include "function/render/graphics_context.h"
#include "function/render/image.h"
#include "vulkan_swapchain.h"
#include "vulkan_frame_in_flight.h"
#include "vulkan_device.h"
#include "vulkan_descriptor_manager.h"
#include "vulkan_command_manager.h"
#include "vulkan_graphics_pipeline.h"
#include "function/window/window.h"

namespace Zafkiel
{
class VulkanRenderPass;
class VulkanContext final : public GraphicsContext
{
  public:
    VulkanContext(const Window &window);
    ~VulkanContext() { device->GetHandle().waitIdle(); }

    Scope<VulkanDevice> &GetDevice() { return device; }
    Scope<VulkanPhysicalDevice> &GetPhysicalDevice() { return physicalDevice; }
    Scope<VulkanCommandManager> &GetCommandManager() { return commandManager; }
    Scope<VulkanDescriptorManager> &GetDescriptorManager() { return descriptorManager; }
    Scope<RenderPass> &GetMainRenderPass() { return mainRenderPass; }
    vk::raii::Instance &GetInstance() { return instance; }
    vk::raii::SurfaceKHR &GetSurface() { return surface; }
    Scope<VulkanSwapchain> &GetSwapchain() { return swapchain; }
    void RenderToScreen(std::function<void()> renderFunc);
    void Present();
    void InvalidateSwapchain();
    void ResizeSwapchain(uint32_t width, uint32_t height);
    std::vector<vk::DescriptorSet> RegisterImGuiTexture2D(Observer<Texture2D> texture);
    void UnregisterImGuiTexture2D(Observer<Texture2D> texure);

    virtual void Resize(uint32_t width, uint32_t height) override;
    virtual Scope<VertexBuffer> CreateVertexBuffer(const float *vertices, uint32_t size) const override;
    virtual Scope<IndexBuffer> CreateIndexBuffer(const uint32_t *indices, uint32_t count) const override;
    virtual Scope<Mesh> CreateMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices) const override;
    virtual Scope<FrameBuffer> CreateFrameBuffer(const FrameBufferSpecification &spec) const override;
    virtual Scope<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecification &spec) const override;
    virtual Scope<RenderPass> CreateRenderPass(const RenderPassSpecification &spec) const override;
    virtual Scope<GraphicsShader> CreateGraphicsShader(const Path &path) const override;
    virtual Scope<Material> CreateMaterial(const MaterialSpecification &spec) const override;
    virtual Scope<Image> CreateImage(const ImageSpecification &spec) const override;
    virtual Scope<Texture2D> CreateTexture2D(const Texture2DSpecification &spec) const override;
    virtual Scope<Texture2D> CreateTexture2D(const Texture2DSpecification &spec, Buffer buffer) const override;
    virtual Scope<RenderResourceTemplate> CreateRenderResourceTemplate(const Observer<RenderResourceSchema> schema) const override;
    virtual Scope<RenderResource> CreateRenderResource(const Observer<RenderResourceTemplate> renderResourceTemplate) const override;
    virtual Scope<GlobalRenderResource> CreateGlobalRenderResource(const Path &path) const override;
    virtual Scope<RenderPassResource> CreateRenderPassResource(const Path &path) const override;
    virtual Scope<ObjectRenderResource> CreateObjectRenderResource(const Path &path) const override;
    virtual Scope<CubeMap> CreateCubeMap(const std::vector<Path> &paths) const override { return nullptr; }
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual GraphicsAPI GetAPI() const override { return GraphicsAPI::Vulkan; }

    vk::raii::CommandBuffer &GetCurrentCommandBuffer() { return framesInFlight[curFrame]->cmdBuf; }
    const vk::raii::CommandBuffer &GetCurrentCommandBuffer() const { return framesInFlight[curFrame]->cmdBuf; }

    uint32_t GetCurrentFrame() const { return curFrame; }

  private:
    vk::raii::Context context;
    vk::raii::Instance instance;
    Scope<VulkanPhysicalDevice> physicalDevice;
    Scope<VulkanDevice> device;

    vk::raii::SurfaceKHR surface;
    bool isSurfaceSizeFixed = false;

    Scope<VulkanDescriptorManager> descriptorManager;
    Scope<VulkanCommandManager> commandManager;
    Scope<VulkanSwapchain> swapchain;
    std::vector<Scope<VulkanFrameInFlight>> framesInFlight;
    std::vector<vk::raii::Semaphore> renderFinishedSem;
    Scope<RenderPass> mainRenderPass;

    static constexpr uint32_t flightCount = 2;
    uint32_t curFrame, curImageIndex;

    SDL_Window *window;
    std::vector<const char *> extensions;

    std::unordered_map<VkImage, std::vector<vk::DescriptorSet>> registeredImGuiTextures;

};
}
