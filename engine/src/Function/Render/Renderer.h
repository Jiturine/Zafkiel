#pragma once
#include "Function/RHI/RHI.h"
#include <imgui.h>
#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Application.h"

#include "Function/RHI/ShaderCompiler/GlslCompiler.h"

#include "Function/Render/RenderTargetPool.h"
#include "Function/Render/Mesh.h"
#include "Function/Render/Material.h"

#include "Resource/Asset.h"

namespace Zafkiel
{
class Window;
class MaterialAsset;
class MeshAsset;
class Texture2DAsset;
class FontAsset;

class Renderer
{
  public:
    static Renderer& Instance() { return *instance; }
    ~Renderer();

    static void Init(GraphicsAPI API, Window &window);

    static void Destroy();

    void InitImGui(SDL_Window* window);

    std::vector<ImTextureRef> RegisterImGuiTexture(RHITexture *texture);

    void UnregisterImGuiTexture(RHITexture *texture);

    void DestroyImGui();

    template <typename Fn>
    static void Submit(Fn &&func)
    {
        Application::SubmitToRenderThread(std::forward<Fn>(func));
    }

    RenderTargetPool &GetRenderTargetPool() { return *renderTargetPool.get(); }

    Ref<Mesh> GetOrCreateMesh(AssetHandle meshAssetHandle);

    Ref<Material> GetOrCreateMaterial(AssetHandle materialAssetHandle, const MaterialDesc &desc);

  private:
    Renderer(GraphicsAPI API, Window &window);

    inline static Renderer *instance = nullptr;

    Scope<RenderTargetPool> renderTargetPool;

    std::unordered_map<AssetHandle, Ref<Mesh>> meshes;

    std::unordered_map<AssetHandle, Ref<Material>> materials;

};

}
