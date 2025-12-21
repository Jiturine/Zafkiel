#pragma once
#include "render_resource_template.h"
#include "graphics_context.h"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>
#include "core/application/application.h"
#include "resource/material_asset.h"
#include "resource/mesh_asset.h"

namespace Zafkiel
{
class Window;

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
    const Observer<RenderPass> renderPass;
    const Observer<FrameBuffer> frameBuffer;
    std::vector<ClearValue> clearValues;
};

class Renderer
{
  public:
    static Renderer& Instance() { return *instance; }
    ~Renderer();

    static void Init(GraphicsAPI API, const Window &window)
    {
        instance = Scope<Renderer>(new Renderer(API, window));
    }

    static void Destroy()
    {
        Submit([](){
            instance->DestroyImGui();
            glslang::FinalizeProcess();
        });
        Application::KickRenderThread();
        Application::WaitRenderThread();
        Application::StopRenderThread();
        instance = nullptr;
    }

    static Scope<GraphicsContext> &GetGraphicsContext() { return instance->graphicsContext; }

    void InitImGui(SDL_Window* window);
    std::vector<ImTextureRef> RegisterImGuiTextureImpl(Observer<Texture2D> texture);
    void UnregisterImGuiTextureImpl(Observer<Texture2D> texture);
    static std::vector<ImTextureRef> RegisterImGuiTexture(Observer<Texture2D> texture)
        { return instance->RegisterImGuiTextureImpl(texture); }
    static void UnregisterImGuiTexture(Observer<Texture2D> texture)
        { instance->UnregisterImGuiTextureImpl(texture); }
    void DestroyImGui();

    static void BeginFrame() { instance->BeginFrameImpl();}
    static void EndFrame() { instance->EndFrameImpl(); }

    template <typename Fn>
    static void Submit(Fn &&func)
    {
        Application::SubmitToRenderThread(std::forward<Fn>(func));
    }

    static Observer<RenderResourceTemplate> GetBuiltInMaterialTemplate(ShaderFamily shaderFamily)
    {
        return instance->GetBuiltInMaterialTemplateImpl(shaderFamily);
    }

    static Observer<RenderResourceSchema> GetBuiltInMaterialSchema(ShaderFamily shaderFamily)
    {
        return instance->GetBuiltInMaterialSchemaImpl(shaderFamily);
    }
    
    void LoadBuiltInMaterialTemplates();

    static Observer<Mesh> GetMesh(const Ref<MeshAsset> &meshAsset) { return instance->GetMeshImpl(meshAsset); }

    static Observer<Material> GetMaterial(const Ref<MaterialAsset> &materialAsset) { return instance->GetMaterialImpl(materialAsset); }
    
    static Observer<Texture2D> GetTexture2D(const Ref<Texture2DAsset> &texture2DAsset) { return instance->GetTexture2DImpl(texture2DAsset); }

  private:
    Renderer(GraphicsAPI API, const Window &window);

    inline static Scope<Renderer> instance = nullptr;

    void BeginFrameImpl();
    void EndFrameImpl();
    Observer<RenderResourceTemplate> GetBuiltInMaterialTemplateImpl(ShaderFamily shaderFamily)
    {
        return builtInMaterialTemplates[shaderFamily];        
    }

    Observer<RenderResourceSchema> GetBuiltInMaterialSchemaImpl(ShaderFamily shaderFamily)
    {
        return builtInMaterialSchemas[shaderFamily];        
    }

    Observer<Mesh> GetMeshImpl(const Ref<MeshAsset> &meshAsset);
    Observer<Material> GetMaterialImpl(const Ref<MaterialAsset> &materialAsset);
    Observer<Texture2D> GetTexture2DImpl(const Ref<Texture2DAsset> &texture2DAsset);

    Scope<GraphicsContext> graphicsContext;

    std::unordered_map<ShaderFamily, Scope<RenderResourceSchema>> builtInMaterialSchemas;
    std::unordered_map<ShaderFamily, Scope<RenderResourceTemplate>> builtInMaterialTemplates;
    std::unordered_map<AssetHandle, Scope<Mesh>> meshes;
    std::unordered_map<AssetHandle, Scope<Material>> materials;
    std::unordered_map<AssetHandle, Scope<Texture2D>> texture2Ds;
};

}
