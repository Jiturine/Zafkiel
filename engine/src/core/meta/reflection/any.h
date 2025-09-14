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
        requires(!std::is_same_v<T, Any> && !std::is_same_v<std::decay_t<T>, std::unique_ptr<typename std::decay_t<T>::element_type>>)
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
    template <typename T>
        requires(!std::is_same_v<T, Any>)
    Any(std::unique_ptr<T> ptr)
    {
        any_ = std::shared_ptr<T>(std::move(ptr));
    }
    Any(std::nullptr_t) : any_(nullptr) {}

    Any(const Any &other) = default;
    Any(Any &&other) = default;

    template <typename T>
    T &As()
    {
        if (T **ptr = std::any_cast<T *>(&any_)) return **ptr;
        if (auto ref = std::any_cast<std::reference_wrapper<T>>(&any_)) return ref->get();
        if (auto sptr = std::any_cast<std::shared_ptr<T>>(&any_)) return **sptr;
        throw std::bad_any_cast();
    }

    template <typename T>
    const T &As() const
    {
        if (const T *const *ptr = std::any_cast<const T *>(&any_)) return **ptr;
        if (const T *const *ptr = std::any_cast<T *>(&any_)) return **ptr;
        if (auto ref = std::any_cast<std::reference_wrapper<const T>>(&any_)) return ref->get();
        if (auto ref = std::any_cast<std::reference_wrapper<T>>(&any_)) return ref->get();
        if (const auto ptr = std::any_cast<std::shared_ptr<T>>(&any_)) return **ptr;
        if (const auto ptr = std::any_cast<std::shared_ptr<const T>>(&any_)) return **ptr;
        throw std::bad_any_cast();
    }

  private:
    std::any any_;
};