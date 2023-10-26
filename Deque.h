#pragma once

#include <initializer_list>
#include "Block.h"
#include "RingBuffer.h"

namespace IntDeque {
class Deque {
    using Block = IntBlock::Block;

public:
    Deque();
    explicit Deque(size_t size);
    Deque(std::initializer_list<int> list);

    Deque(const Deque &rhs) = default;
    Deque(Deque &&rhs) noexcept = default;

    Deque &operator=(const Deque &rhs) = default;
    Deque &operator=(Deque &&rhs) noexcept = default;

    void Swap(Deque &rhs);

    void PushBack(int value);
    void PopBack();

    void PushFront(int value);
    void PopFront();

    int &operator[](size_t ind);
    int operator[](size_t ind) const;

    size_t Size() const;

    void Clear();

private:
    RingBuffer::RingBuffer<Block> buf_;
    size_t size_;
};
}  // namespace IntDeque
