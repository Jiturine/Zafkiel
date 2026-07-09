#include "Function/UI/Docking/DockSplitter.h"
#include "Function/UI/Docking/DockTabStack.h"

namespace Zafkiel
{

void DockSplitter::Construct(LayoutRule layoutRule)
{
    content
    .SetSizeRule(SizeRule::Grow)
    [
        splitter = CreateWidget<Splitter>(layoutRule)
    ];
}

DockNode::CleanUpResult DockSplitter::CleanUpNodes() 
{
    CleanUpResult result = CleanUpResult::NoTabsUnderNode;
    for (uint32 index = 0; index < children.size();)
    {
        auto &childNode = children[index];
        auto childCleanUpResult = childNode->CleanUpNodes();
        if (childCleanUpResult == CleanUpResult::VisibleTabsUnderNode || result == CleanUpResult::VisibleTabsUnderNode)
        {
            result = CleanUpResult::VisibleTabsUnderNode;
        }

        switch (childNode->GetNodeType())
        {
        case DockNode::Type::DockTabStack:
        {
            auto childTabStack = childNode.As<DockTabStack>();
            if (childCleanUpResult == CleanUpResult::NoTabsUnderNode)
            {
                RemoveChildAt(index);
            }
            else
            {
                index++;
            }
            break;
        }
        case DockNode::Type::DockSplitter:
        {
            auto childSplitter = childNode.As<DockSplitter>();
            if (childCleanUpResult == CleanUpResult::NoTabsUnderNode)
            {
                RemoveChildAt(index);
            }
            else
            {
                if (childSplitter->children.size() == 1 || childSplitter->GetLayoutRule() == this->GetLayoutRule())
                {
                    auto grandChildSizeFactorScale = splitter->GetChild(index).sizeFactor / childSplitter->ComputeChildSizeFactorTotal();
                    RemoveChildAt(index);
                    for (uint32 grandChildIndex = 0; grandChildIndex < childSplitter->children.size(); grandChildIndex++)
                    {
                        auto grandChild = childSplitter->children[grandChildIndex];
                        InsertChild(grandChild, index, childSplitter->splitter->GetChild(grandChildIndex).sizeFactor * grandChildSizeFactorScale);
                        index++;
                    }
                }
                else
                {
                    index++;
                }
            }
            break;
        }
        default:
            Log::Error("Unknown DockSplitter Child!");
            break;
        }
    }

    if (children.size() == 1 && (children[0]->GetNodeType() == Type::DockSplitter || children[0]->GetNodeType() == Type::DockSpace))
    {
        auto singleChild = children[0].As<DockSplitter>();
        RemoveChildAt(0);
        splitter->SetLayoutRule(singleChild->GetLayoutRule());
        
        auto grandChildSizeFactorScale = splitter->GetChild(0).sizeFactor / singleChild->ComputeChildSizeFactorTotal();

        for (uint32 grandChildIndex = 0; grandChildIndex < singleChild->children.size(); grandChildIndex++)
        {
            auto grandChild = singleChild->children[grandChildIndex];
            AddChild(grandChild, singleChild->splitter->GetChild(grandChildIndex).sizeFactor * grandChildSizeFactorScale);
        }
    }

    return result;
}

void DockSplitter::PlaceNode(const Ref<DockNode> &node, DockTargetDirection direction, const Ref<DockNode> &relativeNode)
{
    if (!DirectionMatchLayoutRule(direction))
    {
        LayoutRule newLayoutRule = splitter->GetLayoutRule() == LayoutRule::Horizontal ? LayoutRule::Vertical : LayoutRule::Horizontal;
        if (children.size() == 1)
        {
            splitter->SetLayoutRule(newLayoutRule);
        }
        else
        {
            auto newSplitter = CreateWidget<DockSplitter>(newLayoutRule);
            ReplaceNode(relativeNode, newSplitter);
            newSplitter->AddChild(relativeNode);
            newSplitter->PlaceNode(node, direction, relativeNode);
            return;
        }
    }

    uint32 index = std::find(children.begin(), children.end(), relativeNode) - children.begin();

    if (direction == DockTargetDirection::Left || direction == DockTargetDirection::Top)
    {
        InsertChild(node, index);
    }
    else
    {
        InsertChild(node, index + 1);
    }
}

void DockSplitter::ReplaceNode(const Ref<DockNode> &oldNode, const Ref<DockNode> &newNode)
{
    uint32 index = std::find(children.begin(), children.end(), oldNode) - children.begin();

    RemoveChildAt(index);
    InsertChild(newNode, index);
}

float DockSplitter::ComputeChildSizeFactorTotal() const 
{
    float result = 0;
    for (uint32 index = 0; index < children.size(); index++)
    {
        result += splitter->GetChild(index).sizeFactor;
    }
    return result;
}

bool DockSplitter::DirectionMatchLayoutRule(DockTargetDirection direction)
{
    return ((direction == DockTargetDirection::Left || direction == DockTargetDirection::Right) && splitter->GetLayoutRule() == LayoutRule::Horizontal) ||
           ((direction == DockTargetDirection::Top || direction == DockTargetDirection::Bottom) && splitter->GetLayoutRule() == LayoutRule::Vertical);
}

}