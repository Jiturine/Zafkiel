#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "Platform/Filesystem/Filesystem.h"

namespace Zafkiel::Reflection 
{
inline void Register_Platform_Filesystem_Filesystem() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::Path>("Path")
    
    ;
}
}