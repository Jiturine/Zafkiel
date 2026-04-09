#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "editor/Project/Project.h"

namespace Zafkiel::Reflection 
{
inline void Register_editor_Project_Project() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::ProjectConfig>("ProjectConfig")
    .AddProperty(&::Zafkiel::ProjectConfig::name, "name")
    .AddProperty(&::Zafkiel::ProjectConfig::startScene, "startScene")
    .AddProperty(&::Zafkiel::ProjectConfig::assetDirectory, "assetDirectory")
    .AddProperty(&::Zafkiel::ProjectConfig::libraryDirectory, "libraryDirectory")
    ;
}
}