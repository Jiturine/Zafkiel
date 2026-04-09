#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "Core/Base/UUID.h"

namespace Zafkiel::Reflection 
{
inline void Register_Core_Base_UUID() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::UUID>("UUID")
    
    ;
}
}