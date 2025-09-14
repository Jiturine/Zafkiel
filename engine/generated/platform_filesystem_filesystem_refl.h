#pragma once
#include "core/meta/reflection/refl.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel::Reflection 
{
inline void Register_platform_filesystem_filesystem() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::Path>("Path")
    
    ;
}
}