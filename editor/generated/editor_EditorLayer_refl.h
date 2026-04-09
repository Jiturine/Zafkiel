#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "editor/EditorLayer.h"

namespace Zafkiel::Reflection 
{
inline void Register_editor_EditorLayer() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::EditorConfig>("EditorConfig")
    .AddProperty(&::Zafkiel::EditorConfig::startProjectPath, "startProjectPath")
    ;
}
}