#include "buffer.h"

namespace Zafkiel
{
Buffer::Buffer(size_t size) : data_(size) {}

Buffer::Buffer(const void *data, size_t size)
    : data_(static_cast<const uint8_t *>(data),
          static_cast<const uint8_t *>(data) + size) {}

Buffer::Buffer(const std::vector<uint8_t> &data) : data_(data) {}

Buffer::Buffer(Buffer &&other) noexcept
    : data_(std::move(other.data_)) {}

Buffer &Buffer::operator=(Buffer &&other) noexcept
{
    if (this != &other)
    {
        data_ = std::move(other.data_);
    }
    return *this;
}

void Buffer::clear()
{
    data_.clear();
}

bool Buffer::empty() const
{
    return data_.empty();
}

size_t Buffer::size() const
{
    return data_.size();
}

size_t Buffer::capacity() const
{
    return data_.capacity();
}

void Buffer::reserve(size_t new_capacity)
{
    data_.reserve(new_capacity);
}

void Buffer::resize(size_t new_size)
{
    data_.resize(new_size);
}

void Buffer::shrink_to_fit()
{
    data_.shrink_to_fit();
}

const uint8_t *Buffer::data() const
{
    return data_.data();
}

uint8_t *Buffer::data()
{
    return data_.data();
}

const uint8_t *Buffer::cdata() const
{
    return data_.data();
}

void Buffer::append(const void *data, size_t size)
{
    if (data && size > 0)
    {
        const auto *byte_data = static_cast<const uint8_t *>(data);
        data_.insert(data_.end(), byte_data, byte_data + size);
    }
}

void Buffer::append(const Buffer &other)
{
    data_.insert(data_.end(), other.data_.begin(), other.data_.end());
}

void Buffer::append(const std::vector<uint8_t> &data)
{
    data_.insert(data_.end(), data.begin(), data.end());
}

void Buffer::append(const std::string &str)
{
    data_.insert(data_.end(), str.begin(), str.end());
}

bool Buffer::read(void *dest, size_t size, size_t offset) const
{
    if (!dest || offset + size > data_.size())
    {
        return false;
    }

    std::memcpy(dest, data_.data() + offset, size);
    return true;
}

Buffer Buffer::clone() const
{
    return Buffer(data_.data(), data_.size());
}

bool Buffer::equals(const Buffer &other) const
{
    if (data_.size() != other.data_.size())
    {
        return false;
    }
    return std::memcmp(data_.data(), other.data_.data(), data_.size()) == 0;
}

bool Buffer::operator==(const Buffer &other) const
{
    return equals(other);
}

bool Buffer::operator!=(const Buffer &other) const
{
    return !equals(other);
}

void Buffer::check_range(size_t offset, size_t size) const
{
    if (offset + size > data_.size())
    {
        throw std::out_of_range("Buffer range out of bounds");
    }
}
}