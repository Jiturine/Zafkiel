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
    requires std::is_base_of_v<RefCounted, T>
class Ref
{
  public:
    template <typename U>
        requires std::is_base_of_v<RefCounted, U>
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
        if (this == &other) return *this;

        other.IncreaseRef();
        DecreaseRef();
        instance = other.instance;
        return *this;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref &operator=(const Ref<T2> &other)
    {
        if (this == &other) return *this;

        other.IncreaseRef();
        DecreaseRef();
        instance = other.instance;
        return *this;
    }

    Ref &operator=(Ref<T> &&other)
    {
        DecreaseRef();
        instance = other.instance;
        other.instance = nullptr;
        return *this;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref &operator=(Ref<T2> &&other)
    {
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
        return instance;
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
        instance = nullptr;
    }

    template <typename T2>
        requires std::is_base_of_v<T, T2>
    Ref<T2> As() const
    {
        return Ref<T2>(*this);
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
    requires std::is_base_of_v<RefCounted, T>
Ref<T> MakeRef(Args &&...args)
{
    return Ref<T>(new T(std::forward<Args>(args)...));
}

}