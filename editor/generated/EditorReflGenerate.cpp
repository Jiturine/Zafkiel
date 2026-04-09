#include "editor_EditorLayer_refl.h"
#include "editor_Project_Project_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEditor() {
    Register_editor_EditorLayer();
    Register_editor_Project_Project();
}
}