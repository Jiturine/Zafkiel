#include "serializer_base.h"

namespace Zafkiel
{
bool DeserializerRef::IsNull() const
{
    return ptr->IsNull();
}
bool DeserializerRef::IsMap() const
{
    return ptr->IsMap();
}
bool DeserializerRef::IsSeq() const
{
    return ptr->IsSeq();
}
bool DeserializerRef::IsScalar() const
{
    return ptr->IsScalar();
}
size_t DeserializerRef::Size() const
{
    return ptr->Size();
}
std::vector<std::pair<DeserializerRef, DeserializerRef>> DeserializerRef::MapItems() const
{
    return ptr->MapItems();
}
std::vector<DeserializerRef> DeserializerRef::SeqElems() const
{
    return ptr->SeqElems();
}
const IDeserializer &DeserializerRef::operator[](size_t index) const
{
    return (*ptr)[index];
}

}
