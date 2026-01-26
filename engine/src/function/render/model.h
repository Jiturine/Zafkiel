#pragma once
#include "core/base/memory.h"
#include "resource/asset.h"
#include "function/render/mesh.h"
#include "function/render/surface_material.h"

namespace Zafkiel
{

struct ModelMesh
{
    RenderHandle mesh;
    RenderHandle materials;
};

struct ModelNode
{
    std::string name;
    mat4 localTransform;
    std::vector<ModelMesh> meshes;
    std::vector<ModelNode> children;
};

class Model
{
  public:
    Model(ModelNode rootNode) : rootNode(rootNode) {}

    const ModelNode &GetRootNode() const { return rootNode; }

  private:
    ModelNode rootNode;
};

}