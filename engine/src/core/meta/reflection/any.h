#pragma once

class Any 
{
  public:
    template <typename T>
    Any(T obj) : any_(obj) {}

    Any(std::nullptr_t) : any_(nullptr) {}

    Any() : any_(nullptr) {}

    template <typename T>
    T &As()
    {
        if (auto ptr = std::any_cast<T>(&any_)) return *ptr;
        throw std::bad_any_cast();
    }

    template <typename T>
    const T &As() const
    {
        if (auto ptr = std::any_cast<const T>(&any_)) return *ptr;
        if (auto ptr = std::any_cast<T>(&any_)) return *ptr;
        throw std::bad_any_cast();
    }

  private:
    std::any any_;
};

class AnyRef
{
  public:
    template <typename T>
        requires(!std::is_same_v<T, AnyRef>)
    AnyRef(T &ref) : any_(std::ref(ref))
    {
    }
    template <typename T>
        requires(!std::is_same_v<T, AnyRef>)
    AnyRef(const T &ref) : any_(std::cref(ref))
    {
    }

    AnyRef(Any &any) : any_(std::ref(any))
    {
    }

    AnyRef(const Any &any) : any_(std::cref(any))
    {
    }
    
    AnyRef(std::nullptr_t) : any_(nullptr) {}

    AnyRef(const AnyRef &other) = default;
    AnyRef(AnyRef &&other) = default;

    template <typename T>
    T &As()
    {
        if (auto ref = std::any_cast<std::reference_wrapper<T>>(&any_)) return ref->get();
        if (auto anyRef = std::any_cast<std::reference_wrapper<Any>>(&any_)) return anyRef->get().As<T>();
        throw std::bad_any_cast();
    }

    template <typename T>
    const T &As() const
    {
        if (auto ref = std::any_cast<std::reference_wrapper<const T>>(&any_)) return ref->get();
        if (auto ref = std::any_cast<std::reference_wrapper<T>>(&any_)) return ref->get();
        if (auto anyRef = std::any_cast<std::reference_wrapper<Any>>(&any_)) return anyRef->get().As<T>();
        throw std::bad_any_cast();
    }

  private:
    std::any any_;
};