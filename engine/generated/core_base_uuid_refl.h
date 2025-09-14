#pragma once
#include "core/meta/reflection/refl.h"
#include "core/base/uuid.h"

namespace Zafkiel::Reflection 
{
inline void Register_core_base_uuid() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::UUID>("UUID")
    
    ;
}
}