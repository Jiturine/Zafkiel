#include "SerializerBase.h"

namespace Zafkiel
{
template <typename T>
T DeserializerRef::As(AnyRef context) const
{
    return ptr->As<T>(context);
}

template <typename T>
    requires std::is_convertible_v<T, std::string>
const IDeserializer &DeserializerRef::operator[](T &&key) const
{
    return (*ptr)[key];
}


}