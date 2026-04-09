#pragma once

namespace Zafkiel 
{
enum class [[refl]] ImageFormat
{
    None = 0,
    R8,
    RG8,
    RGB8,
    RGBA8,
    BGR8,
    BGRA8,
    R8_sRGB,
    RG8_sRGB,
    RGB8_sRGB,
    RGBA8_sRGB,
    BGR8_sRGB,
    BGRA8_sRGB,
    RGB16F,
    RGBA16F,
    RGBA32F,
    R32UI,
    R32F,
    DEPTH24STENCIL8,
    DEPTH32F,
};

uint32 ImageFormatToBytes(ImageFormat format);

uint32 ImageFormatToChannels(ImageFormat format);

}