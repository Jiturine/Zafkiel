#include "Visibility.h"

namespace Zafkiel
{

const Visibility Visibility::Visible(VisibilityEnum::Visible | VisibilityEnum::SelfHitTestVisible | VisibilityEnum::ChildrenHitTestVisible);
const Visibility Visibility::Collapsed(VisibilityEnum::Collapsed);
const Visibility Visibility::Hidden(VisibilityEnum::Hidden);
const Visibility Visibility::HitTestInvisible(VisibilityEnum::Visible);
const Visibility Visibility::SelfHitTestInvisible(VisibilityEnum::Visible | VisibilityEnum::ChildrenHitTestVisible);
const Visibility Visibility::All(VisibilityEnum::Visible | VisibilityEnum::Collapsed | VisibilityEnum::Hidden | VisibilityEnum::SelfHitTestVisible | VisibilityEnum::ChildrenHitTestVisible);

}
