#include "Function/UI/Docking/DockSubWindow.h"
#include "Function/UI/Docking/DockSpace.h"

namespace Zafkiel
{

Ref<DockSpace> DockSubWindow::GetParentDockSpace()
{
    return parentDockSpace.Lock();
}

}
