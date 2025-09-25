#pragma once
#include "core/meta/reflection/refl.h"
#include "editor.h"

namespace Zafkiel::Reflection 
{
inline void Register_editor() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::ProjectConfig>("ProjectConfig")
    .AddProperty(&::Zafkiel::ProjectConfig::name, "name")
    .AddProperty(&::Zafkiel::ProjectConfig::startScene, "startScene")
    .AddProperty(&::Zafkiel::ProjectConfig::assetDirectory, "assetDirectory")
    .AddProperty(&::Zafkiel::ProjectConfig::libraryDirectory, "libraryDirectory")
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::EditorConfig>("EditorConfig")
    .AddProperty(&::Zafkiel::EditorConfig::startProjectPath, "startProjectPath")
    ;
}
}