#pragma once

#include "core/meta/serializer/serializer_base.h"
#include "core/meta/serializer/custom_serialize.h"

namespace Zafkiel 
{
    
enum class BinaryTag : uint8_t 
{
    Null           = 0x00,
                   
    Bool           = 0x01,
    Char           = 0x02,
    Int8           = 0x03,
    Int16          = 0x04,
    Int32          = 0x05,
    Int64          = 0x06,
    UInt8          = 0x07,
    UInt16         = 0x08,
    UInt32         = 0x09,
    UInt64         = 0x10,
    Float          = 0x11,
    Double         = 0x12,
    String         = 0x13,
    Enum           = 0x14,
    Pack           = 0x15,

    MapBegin       = 0x16,
    MapEnd         = 0x17,

    SeqBegin       = 0x18,
    SeqEnd         = 0x19,
};

class BinarySerializer : public ISerializer 
{
  public:
    std::vector<uint8_t> buffer;

    ISerializer& BeginMap() override
    {
        WriteTag(BinaryTag::MapBegin);
        return *this;
    }
    ISerializer& EndMap() override
    {
        WriteTag(BinaryTag::MapEnd);
        return *this;
    }
    ISerializer& BeginSeq() override
    {
        WriteTag(BinaryTag::SeqBegin);
        return *this;
    }
    ISerializer& EndSeq() override
    {
        WriteTag(BinaryTag::SeqEnd);
        return *this;
    }
    ISerializer& BeginBatchSeq() override
    {
        WriteTag(BinaryTag::SeqBegin);
        return *this;
    }
    ISerializer& EndBatchSeq() override
    {
        WriteTag(BinaryTag::SeqEnd);
        return *this;
    }
    
    const std::vector<uint8_t> &GetOutput() { return buffer; }
    
    void WritePack(const void *data, uint32_t size)
    {
        WriteTag(BinaryTag::Pack);
        WriteRaw<uint32_t>(size);
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(data), reinterpret_cast<const uint8_t*>(data) + size);
    }

  private:
    void WriteTag(BinaryTag tag)
    {
        buffer.push_back((uint8_t)tag);
    }

    template<typename T>
    void WriteRaw(const T &val)
    {
        const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(&val);
        buffer.insert(buffer.end(), bytePtr, bytePtr + sizeof(T));
    }
    void WriteData(const void *data, size_t size)
    {
        const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(data);
        buffer.insert(buffer.end(), bytePtr, bytePtr + size);
    }
    
    ISerializer& ValueImpl(const AnyRef key, const Type* typeInfo, AnyRef context) override
    {
        SerializeAny(key, typeInfo, context);
        return *this;
    }

    ISerializer& KeyImpl(const AnyRef key, const Type* typeInfo, AnyRef context) override
    {
        SerializeAny(key, typeInfo, context);
        return *this;
    }
    void SerializeAny(const AnyRef instance, const Type *typeInfo, AnyRef context = nullptr)
    {
        if (auto it = customSerializeOps.find(typeInfo); it != customSerializeOps.end())
        {
            it->second.serializeFunc(instance, context, *this);
            return;
        }
        switch (typeInfo->GetCategory())
        {
        case TypeCategory::Fundamental: SerializeFundamental(instance, typeInfo->As<Fundamental>()); break;
        case TypeCategory::String: SerializeString(instance, typeInfo->As<String>()); break;
        case TypeCategory::Enum: SerializeEnum(instance, typeInfo->As<Enum>()); break;
        case TypeCategory::List: SerializeList(instance, typeInfo->As<List>()); break;
        case TypeCategory::Dict: SerializeDict(instance, typeInfo->As<Dict>()); break;
        case TypeCategory::Class: SerializeClass(instance, typeInfo->As<Class>()); break;
        default: Log::Error("Unknown Type!"); break;
        }
    }

    void SerializeFundamental(const AnyRef value, const Fundamental *typeInfo)
    {
        switch (typeInfo->GetKind())
        {
            using enum FundamentalKind;
        case Bool: WriteTag(BinaryTag::Bool); WriteRaw<bool>(value.As<bool>()); break;
        case Char: WriteTag(BinaryTag::Char); WriteRaw<char>(value.As<char>()); break;
        case Int8: WriteTag(BinaryTag::Int8); WriteRaw<int8_t>(value.As<int8_t>()); break;
        case Int16: WriteTag(BinaryTag::Int16); WriteRaw<int16_t>(value.As<int16_t>()); break;
        case Int32: WriteTag(BinaryTag::Int32); WriteRaw<int32_t>(value.As<int32_t>()); break;
        case Int64: WriteTag(BinaryTag::Int64); WriteRaw<int64_t>(value.As<int64_t>()); break;
        case UInt8: WriteTag(BinaryTag::UInt8); WriteRaw<uint8_t>(value.As<uint8_t>()); break;
        case UInt16: WriteTag(BinaryTag::UInt16); WriteRaw<uint16_t>(value.As<uint16_t>()); break;
        case UInt32: WriteTag(BinaryTag::UInt32); WriteRaw<uint32_t>(value.As<uint32_t>()); break;
        case UInt64: WriteTag(BinaryTag::UInt64); WriteRaw<uint64_t>(value.As<uint64_t>()); break;
        case Float: WriteTag(BinaryTag::Float); WriteRaw<float>(value.As<float>()); break;
        case Double: WriteTag(BinaryTag::Double); WriteRaw<double>(value.As<double>()); break;
        default: break;
        }
    }

    void SerializeString(const AnyRef instance, const String *typeInfo)
    {
        WriteTag(BinaryTag::String);
        auto &str = instance.As<std::string>();
        uint32_t len = str.size();
        WriteRaw<uint32_t>(len);
        WriteData(str.data(), len);
    }

    void SerializeEnum(const AnyRef instance, const Enum *typeInfo)
    {
        WriteTag(BinaryTag::Enum);
        WriteRaw<int32_t>(typeInfo->GetValue(instance));
    }

    void SerializeList(const AnyRef instance, const List *typeInfo)
    {
        BeginSeq();
        for (size_t i = 0; i < typeInfo->GetSize(instance); i++)
        {
            const AnyRef elem = typeInfo->GetElem(i, instance);
            SerializeAny(elem, typeInfo->GetElemType());
        }
        EndSeq();
    }

    void SerializeDict(const AnyRef instance, const Dict *typeInfo)
    {
        BeginMap();
        for (const auto &[key, val] : typeInfo->GetKeyValPairs(instance))
        {
            SerializeAny(key, typeInfo->GetKeyType());
            SerializeAny(val, typeInfo->GetValType());
        }
        EndMap();
    }

    void SerializeProperty(const AnyRef instance, const std::shared_ptr<Property> prop)
    {
        auto propTypeInfo = prop->GetTypeInfo();
        const std::string propName = prop->GetName();
        SerializeString(propName, GetType<std::string>()->As<String>());
        SerializeAny(prop->Call(instance), propTypeInfo);
    }

    void SerializeClass(const AnyRef instance, const Class *typeInfo)
    {
        BeginMap();
        for (auto prop : typeInfo->GetProperties())
        {
            SerializeProperty(instance, prop);
        }
        EndMap();
    }
};

class BinaryDeserializer : public IDeserializer
{
  public:
    BinaryDeserializer(const uint8_t *data, uint32_t size)
        : data(data), size(size)
    {
        if (IsMap())
        {
            uint32_t pos = 1;
            while((BinaryTag)data[pos] != BinaryTag::MapEnd && pos < size)
            {
                uint32_t keyEnd = SkipNode(pos);
                BinaryDeserializer keyDes(data + pos, keyEnd - pos);
                uint32_t valueEnd = SkipNode(keyEnd);
                BinaryDeserializer valueDes(data + keyEnd, valueEnd - keyEnd);
                itemStorage.push_back({std::move(keyDes), std::move(valueDes)});
                pos = valueEnd;
            }
        }
        else if (IsSeq())
        {
            uint32_t pos = 1;
            while((BinaryTag)data[pos] != BinaryTag::SeqEnd && pos < size)
            {
                uint32_t elemEnd = SkipNode(pos);
                BinaryDeserializer elemDes(data + pos, elemEnd - pos);
                elemStorage.push_back(std::move(elemDes));
                pos = elemEnd;
            }
        }
    }

    BinaryDeserializer(Buffer buffer)
        : data(buffer.Data<uint8_t>()), size(buffer.Size<uint8_t>()) {}

    virtual bool IsNull() const override { return (BinaryTag)data[0] == BinaryTag::Null || data == nullptr; }
    virtual bool IsMap() const override { return (BinaryTag)data[0] == BinaryTag::MapBegin; }
    virtual bool IsSeq() const override { return (BinaryTag)data[0] == BinaryTag::SeqBegin; }
    virtual bool IsPack() const override{ return (BinaryTag)data[0] == BinaryTag::Pack; }
    virtual bool IsScalar() const override { return !IsMap() && !IsSeq() && !IsPack() && !IsNull(); }
    virtual std::pair<const uint8_t *, uint32_t> Unpack() const override
    {
        uint32_t size = ReadRaw<uint32_t>(data[1]);
        return { &data[1 + sizeof(uint32_t)], size };
    }

    virtual size_t Size() const override 
    {
        if (IsMap())
        {
            return itemStorage.size();
        }
        else if (IsSeq())
        {
            return elemStorage.size();
        }
        return 1;
    }

    virtual std::vector<DeserializerRef> SeqElems() const override
    {
        std::vector<DeserializerRef> res;
        for (auto &elem : elemStorage)
        {
            res.push_back(&elem);
        }
        return res;
    }

    virtual std::vector<std::pair<DeserializerRef, DeserializerRef>> MapItems() const override
    {
        std::vector<std::pair<DeserializerRef, DeserializerRef>> res;
        for (auto &[key, value] : itemStorage)
        {
            res.push_back({&key, &value});
        }
        return res;
    }

    virtual IDeserializer &GetByIndex(size_t index) const override
    {
        return elemStorage[index];
    }

    virtual IDeserializer &GetByKey(const std::string &key) const override
    {
        for (auto &[keyDes, valueDes] : itemStorage)
        {
            if (keyDes.As<std::string>() == key)
            {
                return valueDes;
            }
        }
        static BinaryDeserializer nullDes(nullptr, 0);
        return nullDes;
    }

    size_t SkipNode(size_t pos) const
    {
        if (pos >= size) return pos;

        BinaryTag tag = (BinaryTag)data[pos++];

        switch (tag)
        {
        case BinaryTag::Bool:  return pos + sizeof(bool);
        case BinaryTag::Char:  return pos + sizeof(char);
        case BinaryTag::Int8:  return pos + sizeof(int8_t);
        case BinaryTag::Int16: return pos + sizeof(int16_t);
        case BinaryTag::Int32: return pos + sizeof(int32_t);
        case BinaryTag::Int64: return pos + sizeof(int64_t);
        case BinaryTag::UInt8: return pos + sizeof(uint8_t);
        case BinaryTag::UInt16:return pos + sizeof(uint16_t);
        case BinaryTag::UInt32:return pos + sizeof(uint32_t);
        case BinaryTag::UInt64:return pos + sizeof(uint64_t);
        case BinaryTag::Float: return pos + sizeof(float);
        case BinaryTag::Double:return pos + sizeof(double);

        case BinaryTag::String:
        {
            uint32_t len = ReadRaw<uint32_t>(data[pos]);
            return pos + sizeof(uint32_t) + len;
        }

        case BinaryTag::Pack:
        {
            uint32_t size = ReadRaw<uint32_t>(data[pos]);
            return pos + sizeof(uint32_t) + size;
        }

        case BinaryTag::SeqBegin:
        {
            while (pos < size && data[pos] != (uint8_t)BinaryTag::SeqEnd)
                pos = SkipNode(pos);
            return pos + 1;
        }

        case BinaryTag::MapBegin:
        {
            while (pos < size && data[pos] != (uint8_t)BinaryTag::MapEnd)
            {
                pos = SkipNode(pos);
                pos = SkipNode(pos);
            }
            return pos + 1;
        }

        default:
            return pos;
        }
    }

    virtual void Value(AnyRef value, const Type *typeInfo, AnyRef context) const override
    {
        DeserializeAny(value, typeInfo, 0, size, context);
    }

    template<typename T>
    const T ReadRaw(const uint8_t &data) const
    {
        return *reinterpret_cast<const T*>(&data);
    }

    BinaryTag ReadTag(const uint8_t &data) const 
    {
        return (BinaryTag)data;
    }

    void AssertTag(BinaryTag tag, uint32_t pos) const
    {
        if ((BinaryTag)data[pos] != tag)
        {
            Log::Error("Tag doesn't match !");
        }
    }

  private:
    void DeserializeFundamental(AnyRef instance, const Fundamental *typeInfo, uint32_t pos, uint32_t size) const
    {
        switch (typeInfo->GetKind())
        {
            using enum FundamentalKind;
        case Bool: AssertTag(BinaryTag::Bool, pos); instance.As<bool>() = ReadRaw<bool>(data[pos + 1]); break;
        case Char: AssertTag(BinaryTag::Char, pos); instance.As<char>() = ReadRaw<char>(data[pos + 1]); break;
        case Int8: AssertTag(BinaryTag::Int8, pos); instance.As<int8_t>() = ReadRaw<int8_t>(data[pos + 1]); break;
        case Int16: AssertTag(BinaryTag::Int16, pos); instance.As<int16_t>() = ReadRaw<int16_t>(data[pos + 1]); break;
        case Int32: AssertTag(BinaryTag::Int32, pos); instance.As<int32_t>() = ReadRaw<int32_t>(data[pos + 1]); break;
        case Int64: AssertTag(BinaryTag::Int64, pos); instance.As<int64_t>() = ReadRaw<int64_t>(data[pos + 1]); break;
        case UInt8: AssertTag(BinaryTag::UInt8, pos); instance.As<uint8_t>() = ReadRaw<uint8_t>(data[pos + 1]); break;
        case UInt16: AssertTag(BinaryTag::UInt16, pos); instance.As<uint16_t>() = ReadRaw<uint16_t>(data[pos + 1]); break;
        case UInt32: AssertTag(BinaryTag::UInt32, pos); instance.As<uint32_t>() = ReadRaw<uint32_t>(data[pos + 1]); break;
        case UInt64: AssertTag(BinaryTag::UInt64, pos); instance.As<uint64_t>() = ReadRaw<uint64_t>(data[pos + 1]); break;
        case Float: AssertTag(BinaryTag::Float, pos); instance.As<float>() = ReadRaw<float>(data[pos + 1]); break;
        case Double: AssertTag(BinaryTag::Double, pos); instance.As<double>() = ReadRaw<double>(data[pos + 1]); break;
        default: break;
        }
    }

    void DeserializeString(AnyRef instance, const String *typeInfo, uint32_t pos, uint32_t size) const
    {
        AssertTag(BinaryTag::String, pos);
        uint32_t len = ReadRaw<uint32_t>(data[pos + 1]);
        instance.As<std::string>() = std::string(data + pos + 1 + sizeof(uint32_t), data + pos + 1 + sizeof(uint32_t) + len);
    }

    void DeserializeEnum(AnyRef instance, const Enum *typeInfo, uint32_t pos, uint32_t size) const
    {
        AssertTag(BinaryTag::Enum, pos);
        typeInfo->SetValue(instance, ReadRaw<int32_t>(data[pos + 1]));
    }

    void DeserializeList(AnyRef instance, const List *typeInfo, uint32_t pos, uint32_t size) const
    {
        AssertTag(BinaryTag::SeqBegin, pos);

        pos++;

        uint32_t count = 0;
        uint32_t tempPos = pos;
        while (ReadTag(data[tempPos]) != BinaryTag::SeqEnd) // 确定元素数量
        {
            tempPos = SkipNode(tempPos);
            count++;
        }

        const auto *elemTypeInfo = typeInfo->GetElemType();
        
        typeInfo->Resize(count, instance); // 重要
        for (size_t i = 0; i < count; i++)
        {
            AnyRef elem = typeInfo->GetElem(i, instance);
            uint32_t elemEnd = SkipNode(pos);
            DeserializeAny(elem, elemTypeInfo, pos, elemEnd - pos);
            pos = elemEnd;
        }
    }

    void DeserializeDict(AnyRef instance, const Dict *typeInfo, uint32_t pos, uint32_t size) const
    {
        BinaryTag endTag;
        AssertTag(BinaryTag::MapBegin, pos);

        pos++;

        uint32_t count = 0;
        uint32_t tempPos = pos;
        while (ReadTag(data[tempPos]) != BinaryTag::MapEnd)
        {
            tempPos = SkipNode(tempPos);
            tempPos = SkipNode(tempPos);
            count++;
        }

        const auto *keyTypeInfo = typeInfo->GetKeyType();
        const auto *valTypeInfo = typeInfo->GetValType();

        for (size_t i = 0; i < count; i++)
        {
            uint32_t keyEnd = SkipNode(pos);
            uint32_t valueEnd = SkipNode(keyEnd);

            Any key = keyTypeInfo->CreateInstance();
            Any val = valTypeInfo->CreateInstance();
            DeserializeAny(key, keyTypeInfo, pos, keyEnd - pos);
            DeserializeAny(val, valTypeInfo, keyEnd, valueEnd - keyEnd);
            typeInfo->Insert(instance, key, val);
        }
    }

    void DeserializeProperty(AnyRef instance, const std::shared_ptr<Property> &prop, uint32_t pos, uint32_t size) const
    {
        AnyRef subInstance = prop->Call(instance);
        DeserializeAny(subInstance, prop->GetTypeInfo(), pos, size);
    }

    void DeserializeClass(AnyRef instance, const Class *typeInfo, uint32_t pos, uint32_t size) const
    {
        AssertTag(BinaryTag::MapBegin, pos);

        pos++;

        while (ReadTag(data[pos]) != BinaryTag::MapEnd)
        {
            uint32_t keyEnd = SkipNode(pos);
            if (ReadTag(data[pos]) != BinaryTag::String)
            {
                pos = keyEnd;
                uint32_t valEnd = SkipNode(pos);
                pos = valEnd;
                continue;
            }
            std::string propName;
            DeserializeString(propName, GetType<std::string>()->As<String>(), pos, keyEnd - pos);

            pos = keyEnd;

            uint32_t valueEnd = SkipNode(pos);
            uint32_t valueStart = pos;

            std::shared_ptr<Property> foundProp = nullptr;
            const auto &props = typeInfo->GetProperties();
            for (const auto &p : props)
            {
                if (p->GetName() == propName)
                {
                    foundProp = p;
                    break;
                }
            }
            if (foundProp)
            {
                AnyRef subInstance = foundProp->Call(instance);
                DeserializeAny(subInstance, foundProp->GetTypeInfo(), valueStart, valueEnd - valueStart);
            }
            pos = valueEnd;
        }
    }
    void DeserializeAny(AnyRef instance, const Type *typeInfo, uint32_t pos, uint32_t size, AnyRef context = nullptr) const
    {
        if (auto it = customSerializeOps.find(typeInfo); it != customSerializeOps.end())
        {
            BinaryDeserializer des(data + pos, size);
            it->second.deserializeFunc(instance, context, des);
            return;
        }
        switch (typeInfo->GetCategory())
        {
        case TypeCategory::Fundamental: DeserializeFundamental(instance, typeInfo->As<Fundamental>(), pos, size); break;
        case TypeCategory::String: DeserializeString(instance, typeInfo->As<String>(), pos, size); break;
        case TypeCategory::Enum: DeserializeEnum(instance, typeInfo->As<Enum>(), pos, size); break;
        case TypeCategory::List: DeserializeList(instance, typeInfo->As<List>(), pos, size); break;
        case TypeCategory::Dict: DeserializeDict(instance, typeInfo->As<Dict>(), pos, size); break;
        case TypeCategory::Class: DeserializeClass(instance, typeInfo->As<Class>(), pos, size); break;
        default: Log::Error("Unknown Type!"); break;
        }
    }

  private:
    const uint8_t *data;
    size_t size;

    mutable std::vector<std::pair<BinaryDeserializer, BinaryDeserializer>> itemStorage;
    mutable std::vector<BinaryDeserializer> elemStorage;
};

}