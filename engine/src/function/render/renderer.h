#pragma once
#include "function/render/shader_material_template.h"
#include "function/render/graphics_context.h"
#include <imgui.h>
#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>
#include "core/application/application.h"

#include "function/render/shader_compiler/glsl_preprocessor.h"
#include "function/render/shader_compiler/glsl_compiler.h"
#include "function/render/render_registry.h"

namespace Zafkiel
{
class Window;
class MaterialAsset;
class MeshAsset;
class Texture2DAsset;

class Renderer
{
  public:
    static Renderer& Instance() { return *instance; }
    ~Renderer();

    static void Init(GraphicsAPI API, const Window &window);

    static void Destroy();

    static Scope<GraphicsContext> &GetGraphicsContext() { return instance->graphicsContext; }

    void InitImGui(SDL_Window* window);
    std::vector<ImTextureRef> RegisterImGuiTexture(RenderHandle texture);
    void UnregisterImGuiTexture(RenderHandle texture);
    void DestroyImGui();
    
    RenderHandle CreateVertexBuffer(const float *vertices, uint32_t size);
    RenderHandle CreateIndexBuffer(const uint32_t *indices, uint32_t count);
    RenderHandle CreateUniformBuffer(uint32_t size);
    RenderHandle CreateMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices);
    RenderHandle CreateGraphicsPipeline(const GraphicsPipelineSpecification &spec);
    RenderHandle CreateVertexModule(Buffer code);
    RenderHandle CreateFragmentModule(Buffer code);
    RenderHandle CreateGraphicsShader(const Path &filePath);
    RenderHandle CreateImage(const ImageSpecification &spec);
    RenderHandle CreateTexture2D(const Texture2DSpecification &spec);
    RenderHandle CreateTexture2D(const Texture2DSpecification &spec, Buffer buffer);
    RenderHandle CreateRenderPass(const RenderPassSpecification &spec);
    RenderHandle CreateFrameBuffer(const FrameBufferSpecification &spec);
    RenderHandle CreateShaderMaterial(RenderHandle shaderMaterialTemplateHandle);
    RenderHandle CreateShaderMaterialTemplate(RenderHandle shaderMaterialSchema);
    RenderHandle CreateShaderMaterialSchema(const Path &path);
    RenderHandle CreateGlobalMaterial(const Path &path);
    RenderHandle CreatePassMaterial(const Path &path);
    RenderHandle CreateObjectShaderMaterial(const Path &path);
    RenderHandle CreateObjectShaderMaterialTemplate(RenderHandle schema);
    RenderHandle CreateSurfaceMaterial(const SurfaceMaterialSpecification &spec, Ref<MaterialAsset> asset);
    
    RenderHandle GetTexture2DFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias);
    void SetTexture2DFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias, RenderHandle texture2D);
    void SetTexture2DFromGlobalMaterial(RenderHandle globalMaterial, const std::string &alias, RenderHandle texture2D);
    void SetTexture2DFromPassMaterial(RenderHandle passMaterial, const std::string &alias, RenderHandle texture2D);
    void SetTexture2DFromSurfaceMaterial(RenderHandle surfaceMaterial, const std::string &alias, RenderHandle texture2D);
    ScopedBuffer GetUniformFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias, ShaderFundamentalType type);
    void SetUniformFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data);
    void SetUniformFromGlobalMaterial(RenderHandle globalMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data);
    void SetUniformFromPassMaterial(RenderHandle passMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data);
    void SetUniformFromSurfaceMaterial(RenderHandle surfaceMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data);
    void SetUniformFromObjectShaderMaterial(RenderHandle objectShaderMaterial, uint32_t index, const std::string &alias, ShaderFundamentalType type, Buffer data);
    void InvalidateImage(RenderHandle handle, const ImageSpecification &spec);
    void InvalidateTexture2D(RenderHandle handle, const Texture2DSpecification &spec);
    void InvalidateFrameBuffer(RenderHandle handle, const FrameBufferSpecification &spec);
    void ResizeImage(RenderHandle handle, uint32_t width, uint32_t height);
    void ResizeTexture2D(RenderHandle handle, uint32_t width, uint32_t height);
    void ResizeFrameBuffer(RenderHandle handle, uint32_t width, uint32_t height);
    
    Borrow<ShaderMaterialSchema> GetShaderMaterialSchema(RenderHandle handle) const { return Borrow(renderRegistry.shaderMaterialSchemas.at(handle)); }
    Borrow<Texture2D> GetTexture2D(RenderHandle handle) const { return Borrow(renderRegistry.texture2Ds.at(handle)); }
    Borrow<Mesh> GetMesh(RenderHandle handle) const { return Borrow(renderRegistry.meshes.at(handle)); }
    Borrow<GlobalMaterial> GetGlobalMaterial(RenderHandle handle) const { return Borrow(renderRegistry.globalMaterials.at(handle)); }
    Borrow<PassMaterial> GetPassMaterial(RenderHandle handle) const { return Borrow(renderRegistry.passMaterials.at(handle)); }
    Borrow<ShaderMaterial> GetShaderMaterial(RenderHandle handle) const { return Borrow(renderRegistry.shaderMaterials.at(handle)); }
    Borrow<ObjectShaderMaterial> GetObjectShaderMaterial(RenderHandle handle) const { return Borrow(renderRegistry.objectShaderMaterials.at(handle)); }
    
    void CmdBindGlobalMaterial(RenderHandle globalMaterial);
    void CmdBindPassMaterial(RenderHandle passMaterial);
    void CmdBindSurfaceMaterial(RenderHandle surfaceMaterial);
    void CmdBindObjectShaderMaterial(uint32_t index, RenderHandle objectMaterial);
    void CmdDrawIndexed(RenderHandle vertexBuffer, RenderHandle indexBuffer);
    void CmdBeginRenderPass(const RenderPassBeginInfo &beginInfo);
    void CmdEndRenderPass();
    void CmdBindGraphicsPipeline(RenderHandle pipeline);
    void UploadObjectShaderMaterialUniform(RenderHandle objectShaderMaterial);

    template <typename Fn>
    static void Submit(Fn &&func)
    {
        Application::SubmitToRenderThread(std::forward<Fn>(func));
    }

    void LoadBuiltInMaterialTemplates();

    RenderHandle GetMeshFromAsset(const Ref<MeshAsset> &meshAsset);
    RenderHandle GetSurfaceMaterialFromAsset(const Ref<MaterialAsset> &materialAsset);
    RenderHandle GetTexture2DFromAsset(const Ref<Texture2DAsset> &texture2DAsset);

    RenderHandle GetBuiltInMaterialTemplate(ShaderFamily shaderFamily)
    {
        return builtInMaterialTemplates[shaderFamily];
    }

    RenderHandle GetBuiltInMaterialSchema(ShaderFamily shaderFamily)
    {
        return builtInMaterialSchemas[shaderFamily];        
    }

    void BeginFrame();
    void EndFrame();

  private:
    Renderer(GraphicsAPI API, const Window &window);

    inline static Renderer *instance = nullptr;
    Scope<RenderRegistryView> renderRegistryView;
    Scope<GraphicsContext> graphicsContext;
    RenderRegistry renderRegistry;

    std::unordered_map<ShaderFamily, RenderHandle> builtInMaterialSchemas;
    std::unordered_map<ShaderFamily, RenderHandle> builtInMaterialTemplates;
    std::unordered_map<AssetHandle, RenderHandle> materialAssetMap;
    std::unordered_map<AssetHandle, RenderHandle> texture2DAssetMap;
    std::unordered_map<AssetHandle, RenderHandle> meshAssetMap;
    
};

}
