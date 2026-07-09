#pragma once
#include "Function/UI/CompoundWidget.h"

namespace Zafkiel
{

class DockSplitter;
class DockSpace;

class DockNode : public CompoundWidget
{
  public:
    enum class Type
    {
        DockSpace,
        DockSplitter,
        DockTabStack,
    };
    
    enum DockTargetDirection
    {
        None, Left, Right, Top, Bottom,
    };

    void SetParentNode(Ref<DockSplitter> parent)
    {
        this->parent = parent;
    }

    virtual Ref<DockSpace> GetDockSpace();

    enum class CleanUpResult
    {
        VisibleTabsUnderNode,
        NoTabsUnderNode
    };

    virtual CleanUpResult CleanUpNodes() { return CleanUpResult::NoTabsUnderNode; }

    virtual Type GetNodeType() const = 0;

  protected:
    WeakRef<DockSplitter> parent;
};

}