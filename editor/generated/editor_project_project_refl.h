#pragma once
#include "core/meta/reflection/refl.h"
#include "editor/project/project.h"

namespace Zafkiel::Reflection 
{
inline void Register_editor_project_project() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::ProjectConfig>("ProjectConfig")
    .AddProperty(&::Zafkiel::ProjectConfig::name, "name")
    .AddProperty(&::Zafkiel::ProjectConfig::startScene, "startScene")
    .AddProperty(&::Zafkiel::ProjectConfig::assetDirectory, "assetDirectory")
    .AddProperty(&::Zafkiel::ProjectConfig::libraryDirectory, "libraryDirectory")
    ;
}
}