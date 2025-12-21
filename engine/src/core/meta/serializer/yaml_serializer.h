#pragma once
#include "serializer_base.h"
#include "custom_serialize.h"

namespace Zafkiel
{

class YamlSerializer : public ISerializer
{
  public:
    virtual ISerializer &BeginMap() override
    {
        if (keyPending) out << YAML::Value;
        out << YAML::BeginMap;
        keyPending = false;
        return *this;
    }

    virtual ISerializer &EndMap() override
    {
        out << YAML::EndMap;
        return *this;
    }

    virtual ISerializer &BeginSeq() override
    {
        if (keyPending) out << YAML::Value;
        out << YAML::BeginSeq;
        keyPending = false;
        return *this;
    }

    virtual ISerializer &EndSeq() override
    {
        out << YAML::EndSeq;
        return *this;
    }

    virtual ISerializer &BeginBatchSeq() override
    {
        if (keyPending) out << YAML::Value;
        out << YAML::Flow << YAML::BeginSeq;
        keyPending = false;
        return *this;
    }
    
    virtual ISerializer &EndBatchSeq() override
    {
        out << YAML::EndSeq;
        return *this;
    }

    virtual ISerializer &KeyImpl(const AnyRef key, const Type *typeInfo, AnyRef context) override
    {
        out << YAML::Key;
        SerializeAny(key, typeInfo, context);
        keyPending = true;
        return *this;
    }

    virtual ISerializer &ValueImpl(const AnyRef value, const Type *typeInfo, AnyRef context) override
    {
        if (keyPending) out << YAML::Value;
        SerializeAny(value, typeInfo, context);
        keyPending = false;
        return *this;
    }

    const char *c_str() const
    {
        return out.c_str();
    }

  private:
    YAML::Emitter out;
    bool keyPending = false;

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
        case Bool: out << value.As<bool>(); break;
        case Char: out << value.As<char>(); break;
        case Int8: out << value.As<int8_t>(); break;
        case Int16: out << value.As<int16_t>(); break;
        case Int32: out << value.As<int32_t>(); break;
        case Int64: out << value.As<int64_t>(); break;
        case UInt8: out << value.As<uint8_t>(); break;
        case UInt16: out << value.As<uint16_t>(); break;
        case UInt32: out << value.As<uint32_t>(); break;
        case UInt64: out << value.As<uint64_t>(); break;
        case Float: out << value.As<float>(); break;
        case Double: out << value.As<double>(); break;
        default: break;
        }
    }

    void SerializeString(const AnyRef instance, const String *typeInfo)
    {
        out << instance.As<std::string>();
    }

    void SerializeEnum(const AnyRef instance, const Enum *typeInfo)
    {
        out << typeInfo->GetValueName(instance);
    }

    void SerializeList(const AnyRef instance, const List *typeInfo)
    {
        out << YAML::BeginSeq;
        for (size_t i = 0; i < typeInfo->GetSize(instance); i++)
        {
            const AnyRef elem = typeInfo->GetElem(i, instance);
            SerializeAny(elem, typeInfo->GetElemType());
        }
        out << YAML::EndSeq;
    }

    void SerializeDict(const AnyRef instance, const Dict *typeInfo)
    {
        out << YAML::BeginMap;
        for (const auto &[key, val] : typeInfo->GetKeyValPairs(instance))
        {
            out << YAML::Key;
            SerializeAny(key, typeInfo->GetKeyType());
            out << YAML::Value;
            SerializeAny(val, typeInfo->GetValType());
        }
        out << YAML::EndMap;
    }

    void SerializeProperty(const AnyRef instance, const std::shared_ptr<Property> prop)
    {
        auto propTypeInfo = prop->GetTypeInfo();
        out << YAML::Key << prop->GetName();
        out << YAML::Value;
        SerializeAny(prop->Call(instance), propTypeInfo);
    }

    void SerializeClass(const AnyRef instance, const Class *typeInfo)
    {
        out << YAML::BeginMap;
        for (auto prop : typeInfo->GetProperties())
        {
            SerializeProperty(instance, prop);
        }
        out << YAML::EndMap;
    }
};

class YamlDeserializer : public IDeserializer
{
  public:
    YamlDeserializer() = default;
    YamlDeserializer(const YAML::Node &node) : node(node) {}
    YamlDeserializer(const std::string &str) { node = YAML::Load(str); }

    virtual bool IsNull() const override { return node.IsNull(); }
    virtual bool IsMap() const override { return node.IsMap(); }
    virtual bool IsSeq() const override { return node.IsSequence(); }
    virtual bool IsScalar() const override { return node.IsScalar(); }
    virtual bool IsPack() const override { return false; }

    virtual size_t Size() const override
    {
        return node.IsNull() ? 0 : node.size();
    }

    virtual std::pair<const uint8_t *, uint32_t> Unpack() const override
    {
        return {nullptr, 0}; // Yaml 不支持紧凑 buffer
    }
    
    virtual std::vector<DeserializerRef> SeqElems() const override
    {
        std::vector<DeserializerRef> res;
        for (size_t i = 0; i < node.size(); i++)
        {
            res.push_back(&GetByIndex(i));
        }
        return res;
    }

    virtual std::vector<std::pair<DeserializerRef, DeserializerRef>> MapItems() const override
    {
        if (itemStorage.empty())
        {
            for (auto it : node)
            {
                auto keyDes = YamlDeserializer(it.first);
                auto valueDes = YamlDeserializer(it.second);
                itemStorage.push_back({std::move(keyDes), std::move(valueDes)});
            }
        }
        std::vector<std::pair<DeserializerRef, DeserializerRef>> res;
        for (auto &[key, value] : itemStorage)
        {
            res.push_back({&key, &value});
        }
        return res;
    }

    virtual IDeserializer &GetByIndex(size_t index) const override
    {
        auto it = storage.find(index);
        if (it != storage.end()) return it->second;
        storage[index] = node[index];
        return storage[index];
    }
    virtual IDeserializer &GetByKey(const std::string &key) const override
    {
        auto it = storage.find(key);
        if (it != storage.end()) return it->second;
        storage[key] = node[key].IsDefined() ? node[key] : YAML::Node{};
        return storage[key];
    }

    virtual void Value(AnyRef value, const Type *typeInfo, AnyRef context) const override
    {
        DeserializeAny(value, typeInfo, node, context);
    }

    void DeserializeFundamental(AnyRef instance, const Fundamental *typeInfo, const YAML::Node &data) const
    {
        switch (typeInfo->GetKind())
        {
            using enum FundamentalKind;
        case Bool: instance.As<bool>() = data.as<bool>(); break;
        case Char: instance.As<char>() = data.as<char>(); break;
        case Int8: instance.As<int8_t>() = data.as<int8_t>(); break;
        case Int16: instance.As<int16_t>() = data.as<int16_t>(); break;
        case Int32: instance.As<int32_t>() = data.as<int32_t>(); break;
        case Int64: instance.As<int64_t>() = data.as<int64_t>(); break;
        case UInt8: instance.As<uint8_t>() = data.as<uint8_t>(); break;
        case UInt16: instance.As<uint16_t>() = data.as<uint16_t>(); break;
        case UInt32: instance.As<uint32_t>() = data.as<uint32_t>(); break;
        case UInt64: instance.As<uint64_t>() = data.as<uint64_t>(); break;
        case Float: instance.As<float>() = data.as<float>(); break;
        case Double: instance.As<double>() = data.as<double>(); break;
        default: break;
        }
    }

    void DeserializeString(AnyRef instance, const String *typeInfo, const YAML::Node &data) const
    {
        const auto &str = data.as<std::string>();
        instance.As<std::string>() = str;
    }

    void DeserializeEnum(AnyRef instance, const Enum *typeInfo, const YAML::Node &data) const
    {
        typeInfo->SetValueName(instance, data.as<std::string>());
    }

    void DeserializeList(AnyRef instance, const List *typeInfo, const YAML::Node &data) const
    {
        const auto *elemTypeInfo = typeInfo->GetElemType();
        typeInfo->Resize(data.size(), instance); // 重要
        for (size_t i = 0; i < data.size(); i++)
        {
            AnyRef elem = typeInfo->GetElem(i, instance);
            DeserializeAny(elem, elemTypeInfo, data[i]);
        }
    }

    void DeserializeDict(AnyRef instance, const Dict *typeInfo, const YAML::Node &data) const
    {
        const auto *keyTypeInfo = typeInfo->GetKeyType();
        const auto *valTypeInfo = typeInfo->GetValType();

        for (const auto &kvp : data)
        {
            Any key = keyTypeInfo->CreateInstance();
            Any val = valTypeInfo->CreateInstance();
            DeserializeAny(key, keyTypeInfo, kvp.first);
            DeserializeAny(val, valTypeInfo, kvp.second);
            typeInfo->Insert(instance, key, val);
        }
    }

    void DeserializeProperty(AnyRef instance, const std::shared_ptr<Property> &prop, const YAML::Node &data) const
    {
        AnyRef subInstance = prop->Call(instance);
        DeserializeAny(subInstance, prop->GetTypeInfo(), data);
    }

    void DeserializeClass(AnyRef instance, const Class *typeInfo, const YAML::Node &data) const
    {
        const auto &props = typeInfo->GetProperties();
        for (const auto &prop : props)
        {
            DeserializeProperty(instance, prop, data[prop->GetName()]);
        }
    }
    void DeserializeAny(AnyRef instance, const Type *typeInfo, const YAML::Node &data, AnyRef context = nullptr) const
    {
        if (auto it = customSerializeOps.find(typeInfo); it != customSerializeOps.end())
        {
            YamlDeserializer des(data);
            it->second.deserializeFunc(instance, context, des);
            return;
        }
        switch (typeInfo->GetCategory())
        {
        case TypeCategory::Fundamental: DeserializeFundamental(instance, typeInfo->As<Fundamental>(), data); break;
        case TypeCategory::String: DeserializeString(instance, typeInfo->As<String>(), data); break;
        case TypeCategory::Enum: DeserializeEnum(instance, typeInfo->As<Enum>(), data); break;
        case TypeCategory::List: DeserializeList(instance, typeInfo->As<List>(), data); break;
        case TypeCategory::Dict: DeserializeDict(instance, typeInfo->As<Dict>(), data); break;
        case TypeCategory::Class: DeserializeClass(instance, typeInfo->As<Class>(), data); break;
        default: Log::Error("Unknown Type!"); break;
        }
    }

  private:
    YAML::Node node;
    mutable std::unordered_map<std::variant<size_t, std::string>, YamlDeserializer> storage;
    mutable std::vector<std::pair<YamlDeserializer, YamlDeserializer>> itemStorage;
};

template <typename T>
std::string Serialize(const T &instance)
{
    YamlSerializer out;
    out.Value(instance);
    return out.c_str();
}
template <typename T>
T Deserialize(const std::string &str)
{
    YamlDeserializer data(str);
    return data.As<T>();
}
}
