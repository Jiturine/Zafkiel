#pragma once
#include "core/meta/reflection/refl.h"

namespace Zafkiel
{
using namespace Reflection;

class ISerializer
{
  public:
    virtual ~ISerializer() = default;

    virtual ISerializer &BeginMap() = 0;

    virtual ISerializer &EndMap() = 0;

    virtual ISerializer &BeginSeq() = 0;

    virtual ISerializer &EndSeq() = 0;

    virtual ISerializer &BeginBatchSeq() = 0;

    virtual ISerializer &EndBatchSeq() = 0;

    template <typename T>
    ISerializer &Key(const T &key, AnyRef context = nullptr)
    {
        return KeyImpl(key, GetType<T>(), context);
    }
    ISerializer &Key(const char *key, AnyRef context = nullptr)
    {
        return Key(std::string(key), context);
    }

    template <typename T>
    ISerializer &Value(const T &value, AnyRef context = nullptr)
    {
        return ValueImpl(value, GetType<T>(), context);
    }
    ISerializer &Value(const char *key, AnyRef context = nullptr)
    {
        return Value(std::string(key), context);
    }

  private:
    virtual ISerializer &KeyImpl(const AnyRef key, const Type *typeInfo, AnyRef context) = 0;
    virtual ISerializer &ValueImpl(const AnyRef value, const Type *typeInfo, AnyRef context) = 0;
};

class IDeserializer;
class DeserializerRef 
{
    const IDeserializer *ptr;
  public:
    DeserializerRef(const IDeserializer *t) : ptr(t) {}
    operator const IDeserializer&() const { return *ptr; }
    
    bool IsNull() const;
    bool IsMap() const;
    bool IsSeq() const;
    bool IsScalar() const;
    size_t Size() const;
    std::vector<std::pair<DeserializerRef, DeserializerRef>> MapItems() const;
    std::vector<DeserializerRef> SeqElems() const;

    template <typename T>
    T As(AnyRef context = nullptr) const;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    const IDeserializer &operator[](T &&key) const;

    const IDeserializer &operator[](size_t index) const;

    operator bool() const { return !IsNull(); }
};

class IDeserializer
{
  public:
    virtual ~IDeserializer() = default;

    virtual bool IsNull() const = 0;
    virtual bool IsMap() const = 0;
    virtual bool IsSeq() const = 0;
    virtual bool IsScalar() const = 0;
    virtual bool IsPack() const = 0;

    virtual size_t Size() const = 0;

    virtual std::vector<std::pair<DeserializerRef, DeserializerRef>> MapItems() const = 0;

    virtual std::vector<DeserializerRef> SeqElems() const = 0;

    virtual std::pair<const uint8_t *, uint32_t> Unpack() const = 0;

    template <typename T>
    T As(AnyRef context = nullptr) const
    {
        if (IsNull()) return T{};
        T value;
        Value(value, GetType<T>(), context);
        return value;
    }

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    const IDeserializer &operator[](T &&key) const
    {
        return GetByKey(std::string(std::forward<T>(key)));
    }

    const IDeserializer &operator[](size_t index) const
    {
        return GetByIndex(index);
    }

    operator bool() const
    {
        return !IsNull();
    }
  private:
    virtual IDeserializer &GetByIndex(size_t index) const = 0;
    virtual IDeserializer &GetByKey(const std::string &key) const = 0;
    virtual void Value(AnyRef value, const Type *typeInfo, AnyRef context) const = 0;
};

}

#include "core/meta/serializer/serializer_base.tpp"