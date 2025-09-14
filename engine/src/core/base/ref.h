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
Ref<T> MakeRef(Args &&...args)
{
    if constexpr (!std::is_base_of_v<RefCounted, T>)
        Log::CoreError("Type is not base of Ref!");
    return Ref<T>(new T(std::forward<Args>(args)...));
}

}