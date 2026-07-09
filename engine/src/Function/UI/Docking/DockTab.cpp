#include "Function/UI/Docking/DockTab.h"
#include "Function/UI/Docking/DockTabBar.h"
#include "Function/UI/Rect.h"
#include "Function/UI/UISystem.h"


namespace Zafkiel
{

Ref<DockTabBar> DockTab::GetParent() const
{
    return parentTabBar.Lock();
}

void DockTab::Construct(const std::wstring &titleStr) 
{
    this->tabTitleStr = titleStr;

    SetBackground(BorderBackgroundSlot()
    [
        CreateWidget<Rect>(vec4(0.5f, 0.8f, 0.3f, 1.0f))
    ]);
    SetContent(
        SingleWidgetSlot()
        .SetSizeRule(SizeRule::Grow)
        .SetPadding(Padding{
            .top = 5.0f,
            .bottom = 5.0f,
            .left = 5.0f,
            .right = 5.0f,
        })
        [
            CreateRef<Border>()
            ->SetBackground(
                BorderBackgroundSlot()
                [
                    CreateWidget<Rect>(vec4(0.9f, 0.7f, 0.5f, 1.0f))
                ]
            )
            ->SetContent(
                SingleWidgetSlot()
                .SetSizeRule(SizeRule::Auto)
                .SetVerticalAlignmentRule(VerticalAlignmentRule::Center)
                .SetHorizontalAlignmentRule(HorizontalAlignmentRule::Center)
                [
                    tabTitle = CreateWidget<Text>(tabTitleStr, UISystem::Instance().GetDefaultFont(), 20, vec3(1.0f, 1.0f, 1.0f), 200)
                ]
            )
        ]
    );
}

Reply DockTab::OnMouseButtonDown(WidgetGeometry widgetGeometry, PointerEvent &event)
{
    return Reply::Handled().DetectDrag(this, event.GetEffectingButton());
}

Reply DockTab::OnDragDetected(WidgetGeometry widgetGeometry, PointerEvent &event, vec2 startPosition) 
{
    if (!parentTabBar)
    {
        Log::Error("DockTab doesn't have parent DockTabBar!");
        return Reply::Unhandled();
    }

    vec2 tabGrabOffset = startPosition - widgetGeometry.position;
    vec2 tabGrabOffsetFraction = vec2(
        std::clamp(tabGrabOffset.x / widgetGeometry.size.x, 0.0f, 1.0f),
        std::clamp(tabGrabOffset.y / widgetGeometry.size.y, 0.0f, 1.0f)
    );

    return parentTabBar->StartDraggingTab(this, tabGrabOffsetFraction, event);
}
    
}