#pragma once

#include "any.h"
#include "base_type.h"
#include "register_base_type.h"
#include "variable_traits.h"
#include "make_any.h"

#ifdef __REFLECTION_ENABLE__
    #define reflect clang::annotate("reflect")
    #define noreflect clang::annotate("noreflect")
#else
    #define reflect
    #define noreflect
#endif