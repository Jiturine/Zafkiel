#pragma once

#include "register.h"

#ifdef __REFLECTION_ENABLE__
    #define reflect clang::annotate("reflect")
    #define noreflect clang::annotate("noreflect")
#else
    #define reflect
    #define noreflect
#endif