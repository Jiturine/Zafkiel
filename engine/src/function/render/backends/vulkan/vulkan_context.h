#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL.h>
#include "function/render/global_material.h"
#include "function/render/graphics_context.h"
#include "function/render/shader_material_template.h"
#include "function/render/image.h"
#include "function/render/backends/vulkan/vulkan_swapchain.h"
#include "function/render/backends/vulkan/vulkan_frame_in_flight.h"
#include "function/render/backends/vulkan/vulkan_device.h"
#include "function/render/backends/vulkan/vulkan_descriptor_manager.h"
#include "function/render/backends/vulkan/vulkan_command_manager.h"
#include "function/render/backends/vulkan/vulkan_graphics_pipeline.h"
#include "function/window/window.h"
#include "function/render/render_registry.h"

#include "function/render/backends/vulkan/vulkan_texture.h"
#include "function/render/backends/vulkan/vulkan_index_buffer.h"
#include "function/render/backends/vulkan/vulkan_object_shader_material.h"
#include "function/render/backends/vulkan/vulkan_object_shader_material_template.h"
#include "function/render/backends/vulkan/vulkan_shader.h"
#include "function/render/backends/vulkan/vulkan_shader_material.h"
#include "function/render/backends/vulkan/vulkan_shader_module.h"
#include "function/render/backends/vulkan/vulkan_vertex_buffer.h"

namespace Zafkiel
{
class VulkanRenderPass;
class VulkanContext final : public GraphicsContext
{
  public:
    VulkanContext(const Window &window);
    ~VulkanContext() { device->GetHandle().waitIdle(); }

    Borrow<VulkanDevice> GetDevice() const { return Borrow(device); }
    Borrow<VulkanPhysicalDevice> GetPhysicalDevice() const { return Borrow(physicalDevice); }
    Borrow<VulkanCommandManager> GetCommandManager() const { return Borrow(commandManager); }
    Borrow<VulkanDescriptorManager> GetDescriptorManager() const { return Borrow(descriptorManager); }
    Borrow<RenderPass> GetMainRenderPass() const { return Borrow(mainRenderPass); }
    vk::raii::Instance &GetInstance() { return instance; }
    vk::raii::SurfaceKHR &GetSurface() { return surface; }
    Scope<VulkanSwapchain> &GetSwapchain() { return swapchain; }
    void RenderToScreen(std::function<void()> renderFunc);
    void Present();
    void InvalidateSwapchain();
    void ResizeSwapchain(uint32_t width, uint32_t height);
    std::vector<vk::DescriptorSet> RegisterImGuiTexture2D(Borrow<VulkanImageBackend> imageBackend, Borrow<VulkanTexture2DBackend> texture2DBackend);
    void UnregisterImGuiTexture2D(Borrow<VulkanImageBackend> imageBackend);

    virtual void Resize(uint32_t width, uint32_t height) override;
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual GraphicsAPI GetAPI() const override { return GraphicsAPI::Vulkan; }

    vk::raii::CommandBuffer &GetCurrentCommandBuffer() { return framesInFlight[curFrame]->cmdBuf; }
    const vk::raii::CommandBuffer &GetCurrentCommandBuffer() const { return framesInFlight[curFrame]->cmdBuf; }

    uint32_t GetCurrentFrame() const { return curFrame; }
    vk::CommandBuffer GetCommandBuffer() { return commandBuffer; }

  private:
    virtual Scope<VertexBufferBackend> CreateVertexBufferBackend(const float *vertices, uint32_t size) override;
    virtual Scope<IndexBufferBackend> CreateIndexBufferBackend(const uint32_t *indices, uint32_t count) override;
    virtual Scope<UniformBufferBackend> CreateUniformBufferBackend(uint32_t size) override;
    virtual Scope<FrameBufferBackend> CreateFrameBufferBackend(const FrameBufferSpecification &spec, Borrow<RenderPass> renderPass, const std::vector<Borrow<Image>> &images) override;
    virtual Scope<PipelineBackend> CreatePipelineBackend(const GraphicsPipelineSpecification &spec, Borrow<GraphicsShader> shader, Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule, Borrow<RenderPass> renderPass, Borrow<RenderRegistryView> registryView) override;
    virtual Scope<RenderPassBackend> CreateRenderPassBackend(const RenderPassSpecification &spec) override;
    virtual Scope<ShaderBackend> CreateShaderBackend(Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule) override;
    virtual Scope<ImageBackend> CreateImageBackend(const ImageSpecification &spec) override;
    virtual Scope<Texture2DBackend> CreateTexture2DBackend(const Texture2DSpecification &spec, Borrow<ImageBackend> imageBackend) override;
    virtual Scope<ShaderMaterialBackend> CreateShaderMaterialBackend(Borrow<ShaderMaterialTemplateBackend> shaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema) override;
    virtual Scope<ShaderMaterialTemplateBackend> CreateShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema) override;
    virtual Scope<ObjectShaderMaterialBackend> CreateObjectShaderMaterialBackend(Borrow<ObjectShaderMaterialTemplateBackend> objectShaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema) override;
    virtual Scope<ObjectShaderMaterialTemplateBackend> CreateObjectShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema) override;
    virtual Scope<ShaderModuleBackend> CreateShaderModuleBackend(Buffer buffer, ShaderStage stage) override;
    virtual void SetImageData(Borrow<Image> image, Buffer buffer) override;
    virtual void UploadObjectShaderMaterialUniform(Borrow<ObjectShaderMaterialBackend> objectShaderMaterialBackend, const std::vector<std::optional<ObjectShaderResource>> &resources) override;
    void UpdateShaderMaterialDescriptorSet(Borrow<VulkanShaderMaterialBackend> shaderMaterialBackend, const std::vector<std::optional<ShaderResource>> &resources, Borrow<RenderRegistryView> registryView);
    void UploadShaderMaterialUniform(Borrow<VulkanShaderMaterialBackend> shaderMaterialBackend, const std::vector<std::optional<ShaderResource>> &resources);
  
    virtual void WaitIdle() override;

  private:
    void ExecuteCmd(std::function<void(vk::raii::CommandBuffer&)> func);
    void BindGlobalMaterialInPipeline(Borrow<RenderRegistryView> registryView);
    void BindPassMaterialInPipeline(Borrow<RenderRegistryView> registryView);
    virtual void BindGlobalMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) override;
    virtual void BindPassMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) override;
    virtual void BindSurfaceMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) override;
    virtual void BindObjectShaderMaterial(uint32_t index, Borrow<ObjectShaderMaterial> material, Borrow<RenderRegistryView> registryView) override;
    virtual void DrawIndexed(Borrow<VertexBuffer> vertexBuffer, Borrow<IndexBuffer> indexBuffer) override;
    virtual void BindGraphicsPipeline(Borrow<GraphicsPipeline> pipeline, Borrow<RenderRegistryView> registryView) override;
    virtual void BeginRenderPass(Borrow<RenderPass> renderPass, Borrow<FrameBuffer> frameBuffer, const std::vector<ClearValue> &clearValues) override;
    virtual void EndRenderPass() override;

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
    Scope<RenderPass> mainRenderPass;

    static constexpr uint32_t flightCount = 2;
    uint32_t curFrame, curImageIndex;

    SDL_Window *window;
    std::vector<const char *> extensions;

    std::unordered_map<VkImage, std::vector<vk::DescriptorSet>> registeredImGuiTextures;

    bool globalMaterialChanged = false;
    bool passMaterialChanged = false;
    std::optional<Borrow<ShaderMaterial>> currentGlobalMaterial;
    std::optional<Borrow<ShaderMaterial>> currentPassMaterial;
    std::optional<Borrow<Pipeline>> currentPipeline;
    std::optional<Borrow<FrameBuffer>> currentFrameBuffer;
    vk::CommandBuffer commandBuffer;
}; 

}
