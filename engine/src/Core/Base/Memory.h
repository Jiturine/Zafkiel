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
    uint32 GetRefCount() const
    {
        return refCount.load();
    }
  private:
    mutable std::atomic<uint32> refCount = 0;
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

    T *get()
    {
        return instance;
    }

    const T *get() const
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

class ScopedBuffer 
{
  public:
    ScopedBuffer() = default;
    ScopedBuffer(std::nullptr_t) {}
    ScopedBuffer(const ScopedBuffer&) = delete;
    ScopedBuffer& operator=(const ScopedBuffer&) = delete;
    
    ScopedBuffer(uint32 byteSize)
        : data(new uint8[byteSize]), size(byteSize) 
    {
    }

    ScopedBuffer(const uint8 *data, uint32 size)
        : data(new uint8[size]), size(size)
    {
        memcpy(this->data, data, this->size);
    }

    ~ScopedBuffer()
    {
        Release();
    }
    
    void Release()
    {
        if (data)
        {
            delete[] data;
            data = nullptr;
        }
    }

    ScopedBuffer Clone() const 
    {
        return ScopedBuffer(data, size);
    }

    template <typename T>
    T *Data() { return reinterpret_cast<T *>(data); }

    template <typename T>
    const T *Data() const { return reinterpret_cast<const T *>(data); }

    template <typename T>
    uint32 Size() const { return size / sizeof(T); }

    ScopedBuffer(ScopedBuffer &&other)
        : data(other.data), size(other.size)
    {
        other.data = nullptr;
        other.size = 0;
    }
    ScopedBuffer &operator=(ScopedBuffer &&other)
    {
        Release();
        data = other.data;
        size = other.size;
        other.data = nullptr;
        other.size = 0;
        return *this;
    }

  private:
    uint8 *data = nullptr;
    uint32 size = 0;
};

class Buffer
{
  public:
    Buffer(const uint8 *data, uint32 size)
        : data(data), size(size) {}

    Buffer(uint8 *data, uint32 size)
        : data(data), size(size) {}

    Buffer(const ScopedBuffer &scopedBuffer)
        : data(scopedBuffer.Data<uint8>()), size(scopedBuffer.Size<uint8>()) {}

    template <typename T>
    Buffer(const T &data)
        : data((uint8 *)&data), size(sizeof(T)) {}

    Buffer(std::nullptr_t)
        : data(nullptr), size(0) {}
    
    Buffer(ScopedBuffer &&scopedBuffer) = delete;

    template <typename T>
    const T *Data() const { return reinterpret_cast<const T*>(data); }

    template <typename T>
    uint32 Size() const { return size / sizeof(T); }

    operator bool() const { return data != nullptr; }

  private:
    const uint8 *data;
    uint32 size;
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
}
