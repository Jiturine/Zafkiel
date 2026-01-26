#pragma once
#include "function/render/mesh.h"
#include "function/render/surface_material.h"
#include "function/scene/entity.h"

namespace Zafkiel
{

struct Renderable
{
    uint32_t index;
    EntityID entityID;
    mat4 modelMatrix;
    AssetHandle mesh;
    AssetHandle material;
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