#pragma once
#include "function/render/mesh.h"
#include "function/render/material.h"
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

struct FrameData 
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 cameraPos;

    std::vector<Renderable> renderables;
};

}