#pragma once

namespace Zafkiel
{

class ScopedBuffer 
{
  public:
    ScopedBuffer() = default;
    ScopedBuffer(std::nullptr_t) {}
    ScopedBuffer(const ScopedBuffer&) = delete;
    ScopedBuffer& operator=(const ScopedBuffer&) = delete;
    
    ScopedBuffer(uint32_t byteSize)
        : data(new uint8_t[byteSize]), size(byteSize) 
    {
    }

    ScopedBuffer(const uint8_t *data, uint32_t size)
        : data(new uint8_t[size]), size(size)
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
    uint32_t Size() const { return size / sizeof(T); }

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
    uint8_t *data = nullptr;
    uint32_t size = 0;
};

class Buffer 
{
  public:
    Buffer(uint8_t *data, uint32_t size) 
        : data(data), size(size) {}

    Buffer(const ScopedBuffer &scopedBuffer)
        : data(scopedBuffer.Data<uint8_t>()), size(scopedBuffer.Size<uint8_t>()) {}

    template <typename T>
    Buffer(const T &data)
        : data((uint8_t *)&data), size(sizeof(T)) {}
    
    Buffer(ScopedBuffer &&scopedBuffer) = delete;

    template <typename T>
    const T *Data() const { return reinterpret_cast<const T*>(data); }

    template <typename T>
    uint32_t Size() const { return size / sizeof(T); }


  private:
    const uint8_t *data;
    uint32_t size;
};



}