#pragma once
#include "Function/Render/Mesh.h"
#include "Function/Render/Material.h"
#include "Function/Scene/Entity.h"

namespace Zafkiel
{

struct Renderable
{
    uint32 index;
    EntityID entityID;
    mat4 modelMatrix;
    AssetHandle meshAssetHandle;
    AssetHandle materialAssetHandle;
};

struct DirectionalLight 
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct FrameData 
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 cameraPos;
    
    DirectionalLight directionalLight;

    std::vector<Renderable> renderables;
};

}