#pragma once

#include "Core/Meta/Reflection/Register.h"

#ifdef __REFLECTION_ENABLE__
    #define refl clang::annotate("reflect")
    #define norefl clang::annotate("noreflect")
#else
    #define refl
    #define norefl
#endif
