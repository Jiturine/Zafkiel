#pragma once
#include "Core/Base/Memory.h"
#include "Resource/Asset.h"
#include "Function/Render/Mesh.h"

namespace Zafkiel
{
#if 0
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
#endif

}