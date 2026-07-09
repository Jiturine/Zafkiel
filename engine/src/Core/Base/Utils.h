#pragma once

namespace Zafkiel::Utils
{

void Replace(std::string &source_string, std::string sub_string, const std::string new_string);

void Replace(std::string &source_string, char taget_char, const char new_char);

}

template <typename T>
constexpr std::remove_reference_t<T>&& MoveTemp(T&& Obj) noexcept
{
	using CastType = std::remove_reference_t<T>;

	static_assert(std::is_lvalue_reference_v<T>, "MoveTemp called on an rvalue");
	static_assert(!std::is_same_v<CastType&, const CastType&>, "MoveTemp called on a const object");

	return (CastType&&)Obj;
}

constexpr uint32 AlignUp(uint32 value, uint32 alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}

template<typename Enum>
constexpr bool EnumHasAllFlags(Enum Flags, Enum Contains)
{
	using UnderlyingType = __underlying_type(Enum);
	return ((UnderlyingType)Flags & (UnderlyingType)Contains) == (UnderlyingType)Contains;
}

template<typename Enum>
constexpr bool EnumHasAnyFlags(Enum Flags, Enum Contains)
{
	using UnderlyingType = __underlying_type(Enum);
	return ((UnderlyingType)Flags & (UnderlyingType)Contains) != 0;
}

#define ENUM_CLASS_FLAGS(Enum) \
	inline constexpr Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }

// 用于在类内部为私有枚举定义位运算符的宏
#define ENUM_CLASS_FLAGS_PRIVATE(Enum) \
	friend inline constexpr Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	friend inline constexpr Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	friend inline constexpr Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	friend inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	friend inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	friend inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	friend inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	friend inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }

namespace Zafkiel 
{

template <typename... Args>
class Signal {
  public:
    using Slot = std::function<void(Args...)>;

    void Connect(Slot s) 
    {
        slots.push_back(MoveTemp(s));
    }

    template <typename... TArgs>
    void Emit(TArgs&&... args)
    {
        for (auto& s : slots)
            s(std::forward<TArgs>(args)...);
    }

  private:
    std::vector<Slot> slots;
};

template <typename T, bool managed>
class Singleton;

template <typename T>
class Singleton<T, true>
{
  public:
    template <typename... Args>
    static void Init(Args&&... args)
    {
        instance = new T(std::forward<Args>(args)...);
    }

    static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }

    Singleton(T &&) = delete;
    Singleton(const T &) = delete;
    void operator=(const T &) = delete;

    static T &Instance() { return *instance; }

  protected:
    inline static T *instance = nullptr;

    Singleton() = default;
    virtual ~Singleton() = default;
};

template <typename T>
class Singleton<T, false>
{
  public:
    static T &Instance()
    {
        static T instance;
        return instance;
    }
    Singleton(T &&) = delete;
    Singleton(const T &) = delete;
    void operator=(const T &) = delete;

  protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};
}