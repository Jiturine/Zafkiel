#pragma once
#include "function/render/render_handle.h"

namespace Zafkiel
{

class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
class FrameBuffer;
class GraphicsPipeline;
class GraphicsShader;
class VertexModule;
class FragmentModule;
class Image;
class Mesh;
class Texture2D;
class ShaderMaterial;
class ShaderMaterialTemplate;
class ShaderMaterialSchema;
class ObjectShaderMaterial;
class ObjectShaderMaterialTemplate;
class RenderPass;
class GlobalMaterial;
class PassMaterial;
class SurfaceMaterial;

class RenderRegistry
{
  public:
    // 基础资源 - 最先析构（在最后声明）
    std::unordered_map<RenderHandle, Scope<VertexBuffer>> vertexBuffers;
    std::unordered_map<RenderHandle, Scope<IndexBuffer>> indexBuffers;
    std::unordered_map<RenderHandle, Scope<UniformBuffer>> uniformBuffers;
    std::unordered_map<RenderHandle, Scope<VertexModule>> vertexModules;
    std::unordered_map<RenderHandle, Scope<FragmentModule>> fragmentModules;
    std::unordered_map<RenderHandle, Scope<Image>> images;
    std::unordered_map<RenderHandle, Scope<Mesh>> meshes;
    std::unordered_map<RenderHandle, Scope<ShaderMaterialSchema>> shaderMaterialSchemas;

    // 中间资源 - 依赖基础资源
    std::unordered_map<RenderHandle, Scope<Texture2D>> texture2Ds;
    std::unordered_map<RenderHandle, Scope<GraphicsShader>> graphicsShaders;
    std::unordered_map<RenderHandle, Scope<ShaderMaterialTemplate>> shaderMaterialTemplates;
    std::unordered_map<RenderHandle, Scope<ObjectShaderMaterialTemplate>> objectShaderMaterialTemplates;
    std::unordered_map<RenderHandle, Scope<RenderPass>> renderPasses;

    // 高级资源 - 依赖中间资源
    std::unordered_map<RenderHandle, Scope<FrameBuffer>> frameBuffers;
    std::unordered_map<RenderHandle, Scope<ShaderMaterial>> shaderMaterials;
    std::unordered_map<RenderHandle, Scope<ObjectShaderMaterial>> objectShaderMaterials;
    std::unordered_map<RenderHandle, Scope<GraphicsPipeline>> graphicsPipelines;

    // 最高级资源 - 依赖其他所有资源，最后析构
    std::unordered_map<RenderHandle, Scope<GlobalMaterial>> globalMaterials;
    std::unordered_map<RenderHandle, Scope<PassMaterial>> passMaterials;
    std::unordered_map<RenderHandle, Scope<SurfaceMaterial>> surfaceMaterials;
};

class RenderRegistryView
{
  public:
    RenderRegistryView(RenderRegistry &registry) : renderRegistry(registry) {}

    Borrow<VertexBuffer> GetVertexBuffer(RenderHandle handle) const { return Borrow(renderRegistry.vertexBuffers.at(handle)); }
    Borrow<IndexBuffer> GetIndexBuffer(RenderHandle handle) const { return Borrow(renderRegistry.indexBuffers.at(handle)); }
    Borrow<UniformBuffer> GetUniformBuffer(RenderHandle handle) const { return Borrow(renderRegistry.uniformBuffers.at(handle)); }
    Borrow<FrameBuffer> GetFrameBuffer(RenderHandle handle) const { return Borrow(renderRegistry.frameBuffers.at(handle)); }
    Borrow<GraphicsPipeline> GetGraphicsPipeline(RenderHandle handle) const { return Borrow(renderRegistry.graphicsPipelines.at(handle)); }
    Borrow<GraphicsShader> GetGraphicsShader(RenderHandle handle) const { return Borrow(renderRegistry.graphicsShaders.at(handle)); }
    Borrow<VertexModule> GetVertexModule(RenderHandle handle) const { return Borrow(renderRegistry.vertexModules.at(handle)); }
    Borrow<FragmentModule> GetFragmentModule(RenderHandle handle) const { return Borrow(renderRegistry.fragmentModules.at(handle)); }
    Borrow<Image> GetImage(RenderHandle handle) const { return Borrow(renderRegistry.images.at(handle)); }
    Borrow<Mesh> GetMesh(RenderHandle handle) const { return Borrow(renderRegistry.meshes.at(handle)); }
    Borrow<Texture2D> GetTexture2D(RenderHandle handle) const { return Borrow(renderRegistry.texture2Ds.at(handle)); }
    Borrow<ShaderMaterial> GetShaderMaterial(RenderHandle handle) const { return Borrow(renderRegistry.shaderMaterials.at(handle)); }
    Borrow<ShaderMaterialTemplate> GetShaderMaterialTemplate(RenderHandle handle) const { return Borrow(renderRegistry.shaderMaterialTemplates.at(handle)); }
    Borrow<ShaderMaterialSchema> GetShaderMaterialSchema(RenderHandle handle) const { return Borrow(renderRegistry.shaderMaterialSchemas.at(handle)); }
    Borrow<ObjectShaderMaterial> GetObjectShaderMaterial(RenderHandle handle) const { return Borrow(renderRegistry.objectShaderMaterials.at(handle)); }
    Borrow<ObjectShaderMaterialTemplate> GetObjectShaderMaterialTemplate(RenderHandle handle) const { return Borrow(renderRegistry.objectShaderMaterialTemplates.at(handle)); }
    Borrow<RenderPass> GetRenderPass(RenderHandle handle) const { return Borrow(renderRegistry.renderPasses.at(handle)); }
    Borrow<GlobalMaterial> GetGlobalMaterial(RenderHandle handle) const { return Borrow(renderRegistry.globalMaterials.at(handle)); }
    Borrow<PassMaterial> GetPassMaterial(RenderHandle handle) const { return Borrow(renderRegistry.passMaterials.at(handle)); }
    Borrow<SurfaceMaterial> GetSurfaceMaterial(RenderHandle handle) const { return Borrow(renderRegistry.surfaceMaterials.at(handle)); }
  
    bool IsShaderMaterialTemplate(RenderHandle handle) const { return renderRegistry.shaderMaterialTemplates.contains(handle); }
    bool IsObjectShaderMaterialTemplate(RenderHandle handle) const { return renderRegistry.objectShaderMaterialTemplates.contains(handle); }

  private:
    RenderRegistry &renderRegistry;
};

}
