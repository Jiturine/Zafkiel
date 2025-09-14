#pragma once

namespace Zafkiel::Reflection
{

enum class TypeCategory
{
    Unknown,
    Fundamental,
    String,
    Enum,
    List,
    Dict,
    Class,
    Property
};

class Fundamental;
class String;
class List;
class Dict;
class Enum;
class Class;
class Property;

template <typename>
struct type_traits
{
    static constexpr TypeCategory category = TypeCategory::Unknown;
};

template <>
struct type_traits<Fundamental>
{
    static constexpr TypeCategory category = TypeCategory::Fundamental;
};

template <>
struct type_traits<String>
{
    static constexpr TypeCategory category = TypeCategory::String;
};

template <>
struct type_traits<List>
{
    static constexpr TypeCategory category = TypeCategory::List;
};

template <>
struct type_traits<Dict>
{
    static constexpr TypeCategory category = TypeCategory::Dict;
};

template <>
struct type_traits<Enum>
{
    static constexpr TypeCategory category = TypeCategory::Enum;
};

template <>
struct type_traits<Class>
{
    static constexpr TypeCategory category = TypeCategory::Class;
};

template <>
struct type_traits<Property>
{
    static constexpr TypeCategory category = TypeCategory::Property;
};

// clang-format off
enum class FundamentalKind
{
    Unknown,
    Bool, Char,
    Int8, Int16, Int32, Int64,
    UInt8, UInt16, UInt32, UInt64,
    Float, Double
};
// clang-format on

template <typename>
struct fundamental_traits
{
    static constexpr FundamentalKind kind = FundamentalKind::Unknown;
    static constexpr std::string name = "Unknown";
};

template <>
struct fundamental_traits<bool>
{
    static constexpr FundamentalKind kind = FundamentalKind::Bool;
    static constexpr std::string name = "Bool";
};
template <>
struct fundamental_traits<char>
{
    static constexpr FundamentalKind kind = FundamentalKind::Char;
    static constexpr std::string name = "Char";
};
template <>
struct fundamental_traits<int8_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::Int8;
    static constexpr std::string name = "Int8";
};
template <>
struct fundamental_traits<int16_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::Int16;
    static constexpr std::string name = "Int16";
};
template <>
struct fundamental_traits<int32_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::Int32;
    static constexpr std::string name = "Int32";
};
template <>
struct fundamental_traits<int64_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::Int64;
    static constexpr std::string name = "Int64";
};
template <>
struct fundamental_traits<uint8_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::UInt8;
    static constexpr std::string name = "UInt8";
};
template <>
struct fundamental_traits<uint16_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::UInt16;
    static constexpr std::string name = "UInt16";
};
template <>
struct fundamental_traits<uint32_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::UInt32;
    static constexpr std::string name = "UInt32";
};
template <>
struct fundamental_traits<uint64_t>
{
    static constexpr FundamentalKind kind = FundamentalKind::UInt64;
    static constexpr std::string name = "UInt64";
};
template <>
struct fundamental_traits<float>
{
    static constexpr FundamentalKind kind = FundamentalKind::Float;
    static constexpr std::string name = "Float";
};
template <>
struct fundamental_traits<double>
{
    static constexpr FundamentalKind kind = FundamentalKind::Double;
    static constexpr std::string name = "Double";
};

template <typename>
struct is_list : std::false_type
{
};
template <typename T, typename Alloc>
struct is_list<std::vector<T, Alloc>> : std::true_type
{
};
template <typename T>
inline constexpr bool is_list_v = is_list<T>::value;

template <typename>
struct list_traits;

template <typename T, typename Alloc>
struct list_traits<std::vector<T, Alloc>>
{
    using ElemType = T;
};

template <typename>
struct is_dict : std::false_type
{
};
template <typename Key, typename Val>
struct is_dict<std::unordered_map<Key, Val>> : std::true_type
{
};
template <typename T>
inline constexpr bool is_dict_v = is_dict<T>::value;

template <typename>
struct dict_traits;

template <typename Key, typename Val>
struct dict_traits<std::unordered_map<Key, Val>>
{
    using KeyType = Key;
    using ValType = Val;
};

template <typename>
struct property_traits;

template <typename Class_, typename Value_>
struct property_traits<Value_ Class_::*>
{
    using ClassType = Class_;
    using ValueType = Value_;
};

}
