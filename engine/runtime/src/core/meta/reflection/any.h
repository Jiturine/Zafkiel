#pragma once

class Any
{
  public:
    // 不支持字面量！

    template <typename T>
        requires(!std::is_same_v<T, Any>)
    Any(T &ref) : any_(std::ref(ref))
    {
    }
    template <typename T>
        requires(!std::is_same_v<T, Any>)
    Any(const T &ref) : any_(std::cref(ref))
    {
    }
    template <typename T>
        requires(!std::is_same_v<T, Any>)
    Any(T *ptr) : any_(ptr)
    {
    }
    template <typename T>
        requires(!std::is_same_v<T, Any>)
    Any(const T *ptr) : any_(ptr)
    {
    }
    Any(const Any &other) = default;
    Any(Any &&other) = default;

    template <typename T>
    T &As()
    {
        if (T **ptr = std::any_cast<T *>(&any_)) return **ptr;
        if (auto ref = std::any_cast<std::reference_wrapper<T>>(&any_)) return ref->get();
        throw std::bad_any_cast();
    }

    template <typename T>
    const T &As() const
    {
        if (const T *const *ptr = std::any_cast<const T *>(&any_)) return **ptr;
        if (const T *const *ptr = std::any_cast<T *>(&any_)) return **ptr;
        if (auto ref = std::any_cast<std::reference_wrapper<const T>>(&any_)) return ref->get();
        if (auto ref = std::any_cast<std::reference_wrapper<T>>(&any_)) return ref->get();
        throw std::bad_any_cast();
    }

  private:
    std::any any_;
};