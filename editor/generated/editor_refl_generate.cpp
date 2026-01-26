#include "editor_editor_layer_refl.h"
#include "editor_project_project_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEditor() {
    Register_editor_editor_layer();
    Register_editor_project_project();
}
}