#pragma once

#include <cassert>
#include <cstddef>  // size_t
#include <exception>
#include <vector>

namespace IntDeque {
namespace RingBuffer {
template <typename T>
class RingBuffer {
public:
    RingBuffer(size_t capacity = 1)
        : buffer_(capacity), size_(0), capacity_(capacity), first_elem_(0) {
    }

    RingBuffer(const RingBuffer &rb)
        : buffer_(rb.buffer_),
          size_(rb.size_),
          capacity_(rb.capacity_),
          first_elem_(rb.first_elem_) {
    }

    RingBuffer(RingBuffer &&rb)
        : buffer_(std::move(rb.buffer_)),
          size_(rb.size_),
          capacity_(rb.capacity_),
          first_elem_(rb.first_elem_) {
        rb.size_ = 0;
        rb.capacity_ = 0;
        rb.first_elem_ = 0;
    }

    RingBuffer &operator=(const RingBuffer &rb) {
        if (this == &rb) {
            return *this;
        }
        buffer_ = rb.buffer_;
        size_ = rb.size_;
        capacity_ = rb.capacity_;
        first_elem_ = rb.first_elem_;

        return *this;
    }

    RingBuffer &operator=(RingBuffer &&rb) {
        buffer_ = std::move(rb.buffer_);
        size_ = rb.size_;
        capacity_ = rb.capacity_;
        first_elem_ = rb.first_elem_;

        rb.buffer_.clear();
        rb.buffer_.shrink_to_fit();
        rb.size_ = 0;
        rb.capacity_ = 0;
        rb.first_elem_ = 0;

        return *this;
    }

    void PushBack(T &&elem) {
        if (size_ == capacity_) {
            Realloc(capacity_ * 2);
        }
        buffer_[(first_elem_ + size_) % capacity_] = std::move(elem);
        ++size_;
    }
    void PopBack() {
        if (size_ * 4 == capacity_) {
            Realloc(capacity_ / 2);
        }
        --size_;
    }

    void PushFront(T &&elem) {
        if (size_ == capacity_) {
            Realloc(capacity_ * 2);
        }
        first_elem_ = (first_elem_ - 1) % capacity_;
        buffer_[first_elem_] = std::move(elem);
        ++size_;
    }
    void PopFront() {
        if (size_ * 4 == capacity_) {
            Realloc(capacity_ / 2);
        }
        first_elem_ = (first_elem_ + 1) % capacity_;
        --size_;
    }

    T *Back() {
        return &buffer_[(first_elem_ + size_ - 1) % capacity_];
    }

    T Back() const {
        return buffer_[(first_elem_ + size_ - 1) % capacity_];
    }

    T *Front() {
        return &buffer_[first_elem_];
    }

    T Front() const {
        return buffer_[first_elem_];
    }

    T &operator[](size_t i) {
        return buffer_[(first_elem_ + i) % capacity_];
    }

    const T &operator[](size_t i) const {
        return buffer_[(first_elem_ + i) % capacity_];
    }

    bool IsEmpty() const {
        return size_ == 0;
    }

private:
    void Realloc(size_t capacity) {
        std::vector<T> new_buffer_(capacity);
        for (size_t i = 0; i < size_; ++i) {
            new_buffer_[i] = std::move(buffer_[(first_elem_ + i) % capacity_]);
        }
        buffer_ = std::move(new_buffer_);
        first_elem_ = 0;
        capacity_ = capacity;
    }

    std::vector<T> buffer_;
    size_t size_;
    size_t capacity_;

    size_t first_elem_;
};
}  // namespace RingBuffer
}  // namespace IntDeque
