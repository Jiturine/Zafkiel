#pragma once

namespace Zafkiel
{

class RefCounted
{
  public:
    virtual ~RefCounted() = default;
    void IncreaseRefCount() const
    {
        refCount++;
    }
    void DecreaseRefCount() const
    {
        refCount--;
    }
    uint32_t GetRefCount() const
    {
        return refCount.load();
    }
  private:
    mutable std::atomic<uint32_t> refCount = 0;
};

namespace RefUtils
{

void AddToLiveReferences(void *instance);

void RemoveFromLiveReferences(void *instance);

bool IsLive(void *instance);

}

template <typename T>
class Scope : public std::unique_ptr<T>
{
  public:
    using Base = std::unique_ptr<T>;
    using Base::Base;
    using Base::operator=;

    template <typename U>
    U& As() 
    {
        return *static_cast<U*>(this->get());
    }

    template <typename U>
    const U& As() const 
    {
        return *static_cast<const U*>(this->get());
    }
};

template<typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args)
{
    return Scope<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
class Ref
{
  public:
    template <typename U>
    friend class Ref;

    Ref() : instance(nullptr) {}
    Ref(std::nullptr_t) : instance(nullptr) {}

    Ref(T *instance) : instance(instance)
    {
        IncreaseRef();
    }

    Ref(const Ref<T> &other) : instance(other.instance)
    {
        IncreaseRef();
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref(const Ref<T2> &other) : instance((T *)other.instance)
    {
        IncreaseRef();
    }

    Ref(Ref<T> &&other) : instance(other.instance)
    {
        other.instance = nullptr;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref(Ref<T2> &&other) : instance((T *)other.instance)
    {
        other.instance = nullptr;
    }

    ~Ref()
    {
        DecreaseRef();
    }

  public:
    Ref &operator=(std::nullptr_t)
    {
        DecreaseRef();
        instance = nullptr;
        return *this;
    }

    Ref &operator=(const Ref<T> &other)
    {
        if (instance == other.instance) return *this;

        other.IncreaseRef();
        DecreaseRef();
        instance = other.instance;
        return *this;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref &operator=(const Ref<T2> &other)
    {
        if (instance == other.instance) return *this;

        other.IncreaseRef();
        DecreaseRef();
        instance = other.instance;
        return *this;
    }

    template <typename T2>
    Ref &operator=(Ref<T> &&other) noexcept
    {
        if (instance == other.instance) return *this;

        DecreaseRef();
        instance = other.instance;
        other.instance = nullptr;
        return *this;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref &operator=(Ref<T2> &&other) noexcept
    {
        if (instance == other.instance) return *this;

        DecreaseRef();
        instance = other.instance;
        other.instance = nullptr;
        return *this;
    }

    operator bool() const
    {
        return instance != nullptr;
    }

    T *operator->()
    {
        return instance;
    }

    const T *operator->() const
    {
        return instance;
    }

    T &operator*()
    {
        return *instance;
    }
    const T &operator*() const
    {
        return *instance;
    }

    T *Raw()
    {
        return instance;
    }

    const T *Raw() const
    {
        return instance;
    }

    void Reset(T *instance = nullptr)
    {
        DecreaseRef();
        this->instance = instance;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    bool Is() const
    {
        return dynamic_cast<T2 *>(instance) != nullptr;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref<T2> As() const
    {
        return static_cast<T2 *>(instance);
    }

    bool operator==(const Ref<T> &other) const
    {
        return instance == other.instance;
    }

    bool operator!=(const Ref<T> &other) const
    {
        return instance != other.instance;
    }

  private:
    void IncreaseRef() const
    {
        if (instance)
        {
            instance->IncreaseRefCount();
            RefUtils::AddToLiveReferences((void *)instance);
        }
    }
    void DecreaseRef() const
    {
        if (instance)
        {
            instance->DecreaseRefCount();
            if (instance->GetRefCount() == 0)
            {
                RefUtils::RemoveFromLiveReferences((void *)instance);
                delete instance;
                instance = nullptr;
            }
        }
    }
    mutable T *instance;
};

template <typename T, typename... Args>
Ref<T> CreateRef(Args &&...args)
{
    if constexpr (!std::is_base_of_v<RefCounted, T>)
        Log::Error("Type is not base of Ref!");
    return Ref<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
class WeakRef
{
public:
    WeakRef() = default;

    WeakRef(Ref<T> ref)
    {
        m_Instance = ref.Raw();
    }

    WeakRef(T* instance)
    {
        m_Instance = instance;
    }

    T* operator->() { return m_Instance; }
    const T* operator->() const { return m_Instance; }

    T& operator*() { return *m_Instance; }
    const T& operator*() const { return *m_Instance; }

    bool IsValid() const { return m_Instance ? RefUtils::IsLive(m_Instance) : false; }
    operator bool() const { return IsValid(); }

    template<typename T2>
    WeakRef<T2> As() const
    {
        return WeakRef<T2>(static_cast<T2*>(m_Instance));
    }
private:
    T* m_Instance = nullptr;
};

template<typename T>
class Observer {
    T* ptr = nullptr;

public:
    Observer() = default;
    Observer(std::nullptr_t) : ptr(nullptr) {}
    template<typename U>
        requires std::is_convertible_v<U*, T*>
    Observer(U* p) : ptr(p) {}
    
    Observer(const Scope<T> &s) : ptr(s.get()) {} 
    template <typename U>
        requires std::is_base_of_v<T, U>
    Observer(const Scope<U> &s) : ptr(s.get()) {}

    Observer(Scope<T>&&) = delete;

    template <typename U>
        requires std::is_convertible_v<U*, T*>
    Observer(const Observer<U> &s) : ptr(s.get()) {}
    
    T* get() const { return ptr; }
    T* operator->() { return ptr; }
    T& operator*() { return *ptr; }

    const T* operator->() const { return ptr; }
    const T& operator*() const { return *ptr; }
    
    template<typename U>
    Observer<U> As() 
    {
        return static_cast<U*>(ptr);
    }
    template<typename U>
    Observer<const U> As() const
    {
        return static_cast<const U*>(ptr);
    }

    explicit operator bool() const { return ptr != nullptr; }

    bool operator==(const Observer& other) const { return ptr == other.ptr; }
    bool operator!=(const Observer& other) const { return ptr != other.ptr; }
};

}

namespace std
{
// 哈希函数
template <typename T>
struct hash<Zafkiel::Ref<T>>
{
    std::size_t operator()(const Zafkiel::Ref<T> &ref) const
    {
        return hash<uintptr_t>()((uintptr_t)ref.Raw());
    }
};

template<typename T>
struct hash<Zafkiel::Scope<T>> {
    size_t operator()(Zafkiel::Scope<T> const& s) const noexcept {
        return std::hash<T*>()(s.get());
    }
};

template<typename T>
struct hash<Zafkiel::Observer<T>> {
    size_t operator()(const Zafkiel::Observer<T>& p) const noexcept {
        return std::hash<T*>()(p.get());
    }
};

}