#include "editor/EditorContext/EditorContext.h"

namespace Zafkiel 
{

void EditorContext::SetSelectedEntityImpl(Entity entity)
{
    selectionContext.type = SelectionContext::Type::Entity;
    selectionContext.entity = entity;
}

}
