#include "editor/editor_context/editor_context.h"

namespace Zafkiel 
{

void EditorContext::SetSelectedEntityImpl(Entity entity)
{
    selectionContext.type = SelectionContext::Type::Entity;
    selectionContext.entity = entity;
}

}
