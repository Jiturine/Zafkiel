#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "Core/Base/ImageFormat.h"

namespace Zafkiel::Reflection 
{
inline void Register_Core_Base_ImageFormat() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::ImageFormat>("ImageFormat")
    .Add(::Zafkiel::ImageFormat::None, "None")
    .Add(::Zafkiel::ImageFormat::R8, "R8")
    .Add(::Zafkiel::ImageFormat::RG8, "RG8")
    .Add(::Zafkiel::ImageFormat::RGB8, "RGB8")
    .Add(::Zafkiel::ImageFormat::RGBA8, "RGBA8")
    .Add(::Zafkiel::ImageFormat::BGR8, "BGR8")
    .Add(::Zafkiel::ImageFormat::BGRA8, "BGRA8")
    .Add(::Zafkiel::ImageFormat::R8_sRGB, "R8_sRGB")
    .Add(::Zafkiel::ImageFormat::RG8_sRGB, "RG8_sRGB")
    .Add(::Zafkiel::ImageFormat::RGB8_sRGB, "RGB8_sRGB")
    .Add(::Zafkiel::ImageFormat::RGBA8_sRGB, "RGBA8_sRGB")
    .Add(::Zafkiel::ImageFormat::BGR8_sRGB, "BGR8_sRGB")
    .Add(::Zafkiel::ImageFormat::BGRA8_sRGB, "BGRA8_sRGB")
    .Add(::Zafkiel::ImageFormat::RGB16F, "RGB16F")
    .Add(::Zafkiel::ImageFormat::RGBA16F, "RGBA16F")
    .Add(::Zafkiel::ImageFormat::RGBA32F, "RGBA32F")
    .Add(::Zafkiel::ImageFormat::R32UI, "R32UI")
    .Add(::Zafkiel::ImageFormat::R32F, "R32F")
    .Add(::Zafkiel::ImageFormat::DEPTH24STENCIL8, "DEPTH24STENCIL8")
    .Add(::Zafkiel::ImageFormat::DEPTH32F, "DEPTH32F")
    ;
 

    
}
}