#include "Function/UI/Docking/DockNode.h"
#include "Function/UI/Docking/DockSplitter.h"
#include "Function/UI/Docking/DockSpace.h"

namespace Zafkiel
{

Ref<DockSpace> DockNode::GetDockSpace()
{
    return parent.IsValid() ? parent.Lock()->GetDockSpace() : nullptr;
}

}