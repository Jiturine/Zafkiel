#pragma once
#include "core/meta/reflection/refl.h"
#include "editor/editor_layer.h"

namespace Zafkiel::Reflection 
{
inline void Register_editor_editor_layer() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::EditorConfig>("EditorConfig")
    .AddProperty(&::Zafkiel::EditorConfig::startProjectPath, "startProjectPath")
    ;
}
}