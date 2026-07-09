#pragma once
#include "Function/UI/Splitter.h"
#include "Function/UI/Docking/DockNode.h"

namespace Zafkiel
{

class DockSplitter : public DockNode
{
  public:
    void Construct(LayoutRule layoutRule);

    void AddChild(const Ref<DockNode> &node, float sizeFactor = 1.0f)
    {
        splitter->AddChild(Splitter::Slot().SetSizeFactor(sizeFactor)[node]);
        children.push_back(node);

        node->SetParentNode(this);
    }

    void InsertChild(const Ref<DockNode> &node, uint32 index, float sizeFactor = 1.0f)
    {
        splitter->InsertChild(Splitter::Slot()[node], index);
        children.insert(children.begin() + index, node);

        node->SetParentNode(this);
    }

    void PlaceNode(const Ref<DockNode> &node, DockTargetDirection direction, const Ref<DockNode> &relativeNode);

    void ReplaceNode(const Ref<DockNode> &oldNode, const Ref<DockNode> &newNode);

    void RemoveChild(const Ref<DockNode> &node)
    {
        splitter->RemoveChild(node);
        children.erase(std::find(children.begin(), children.end(), node));
    }

    void RemoveChildAt(uint32 index)
    {
        splitter->RemoveChildAt(index);
        children.erase(children.begin() + index);
    }

    LayoutRule GetLayoutRule() const { return splitter->GetLayoutRule(); }

    virtual CleanUpResult CleanUpNodes() override;

    virtual Type GetNodeType() const override { return Type::DockSplitter; }

  protected:
    bool DirectionMatchLayoutRule(DockTargetDirection direction);
  
    float ComputeChildSizeFactorTotal() const;

    Ref<Splitter> splitter;

    std::vector<Ref<DockNode>> children;
};

}
