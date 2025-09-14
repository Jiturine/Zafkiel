#pragma once

namespace Zafkiel
{

class Buffer
{
  public:
    Buffer() = default;
    explicit Buffer(size_t size);
    Buffer(const void *data, size_t size);
    Buffer(const std::vector<uint8_t> &data);

    Buffer(Buffer &&other) noexcept;
    Buffer &operator=(Buffer &&other) noexcept;

    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;

    void clear();
    bool empty() const;
    size_t size() const;
    size_t capacity() const;
    void reserve(size_t new_capacity);
    void resize(size_t new_size);
    void shrink_to_fit();

    const uint8_t *data() const;
    uint8_t *data();
    const uint8_t *cdata() const;

    void append(const void *data, size_t size);
    void append(const Buffer &other);
    void append(const std::vector<uint8_t> &data);
    void append(const std::string &str);

    template <typename T>
    void append(const T &value)
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "T must be trivially copyable");
        append(&value, sizeof(T));
    }

    bool read(void *dest, size_t size, size_t offset = 0) const;

    template <typename T>
    bool read(T &value, size_t offset = 0) const
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "T must be trivially copyable");
        return read(&value, sizeof(T), offset);
    }

    Buffer clone() const;

    // 比较操作
    bool equals(const Buffer &other) const;
    bool operator==(const Buffer &other) const;
    bool operator!=(const Buffer &other) const;

  private:
    std::vector<uint8_t> data_;

    void check_range(size_t offset, size_t size) const;
};

}