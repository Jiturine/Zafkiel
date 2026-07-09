#pragma once
#include "Function/UI/Slot.h"
#include "Function/UI/Widget.h"

namespace Zafkiel
{

enum LayoutRule
{
    Vertical,
    Horizontal,
};

namespace LayoutUtils
{

ArrangedWidget ArrangeSingleChild(SingleWidgetSlot child, WidgetGeometry allocatedGeometry);

}

}