#pragma once
#include "core/base/memory.h"
#include "resource/asset.h"
#include "mesh.h"
#include "material.h"

namespace Zafkiel
{

struct ModelMesh
{
    Observer<Mesh> mesh;
    Observer<Material> meshes;
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
    Model(ModelNode rootNode) : rootNode(rootNode)
    {
    }
    const ModelNode &GetRootNode() const { return rootNode; }

  private:
    ModelNode rootNode;
};

}