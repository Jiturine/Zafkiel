#include "image.h"

namespace Zafkiel
{

uint32_t ImageFormatToBytes(ImageFormat format)
{
    switch (format)
    {
        using enum ImageFormat;
    case R8: return 1;
    case RG8: return 2;
    case RGB8: return 3;
    case RGBA8: return 4;
    case BGR8: return 3;
    case BGRA8: return 4;
    case R8_sRGB: return 1;
    case RG8_sRGB: return 2;
    case RGB8_sRGB: return 3;
    case RGBA8_sRGB: return 4;
    case BGR8_sRGB: return 3;
    case BGRA8_sRGB: return 4;
    case RGB16F: return 3 * 2;
    case RGBA16F: return 4 * 2;
    case RGBA32F: return 4 * 4;
    case R32UI: return 4;
    case DEPTH24STENCIL8: return 3 + 1;
    default:
        Log::Error("Unsupported Image Format!");
        return 0;
    }
}

}
