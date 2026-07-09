#pragma once
#include "Function/UI/SubWindow.h"

namespace Zafkiel
{

class DockSpace;

class DockSubWindow : public SubWindow
{
  public:
    void SetParentDockSpace(Ref<DockSpace> parent)
    {
        parentDockSpace = parent;
    }

    Ref<DockSpace> GetParentDockSpace();

  private:
    WeakRef<DockSpace> parentDockSpace;
};

}