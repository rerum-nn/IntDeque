#include <algorithm>
#include "Deque.h"

namespace IntDeque {

Deque::Deque() : size_(0) {
    buf_.PushBack(Block());
}

Deque::Deque(size_t size) : size_(size) {
    for (size_t i = 0; i < size; i += Block::kBlockSize) {
        Block b;
        for (size_t j = 0; j < Block::kBlockSize; j++) {
            b.PushBack(0);
        }
        buf_.PushBack(std::move(b));
    }
}

Deque::Deque(std::initializer_list<int> list) {
    for (size_t i = 0; i < list.size(); i += Block::kBlockSize) {
        Block b;
        for (size_t j = 0; i + j < list.size() && j < Block::kBlockSize; ++j) {
            b.PushBack(*(list.begin() + i + j));
        }
        buf_.PushBack(std::move(b));
    }
    size_ = list.size();
}

void Deque::Swap(Deque &rhs) {
    std::swap(buf_, rhs.buf_);
    std::swap(size_, rhs.size_);
}

void Deque::PushBack(int value) {
    Block *last_block = buf_.Back();
    if (last_block->IsUpBorder()) {
        buf_.PushBack(Block());
        last_block = buf_.Back();
    }
    last_block->PushBack(value);
    ++size_;
}

void Deque::PopBack() {
    Block *last_block = buf_.Back();
    if (last_block->IsEmpty()) {
        buf_.PopBack();
        last_block = buf_.Back();
    }
    last_block->PopBack();
    --size_;
}

void Deque::PushFront(int value) {
    Block *first_block = buf_.Front();
    if (first_block->IsDownBorder()) {
        buf_.PushFront(Block());
        first_block = buf_.Front();
    }
    first_block->PushFront(value);
    ++size_;
}

void Deque::PopFront() {
    Block *first_block = buf_.Front();
    if (first_block->IsEmpty()) {
        buf_.PopFront();
        first_block = buf_.Front();
    }
    first_block->PopFront();
    --size_;
}

int &Deque::operator[](size_t ind) {
    Block *first_block = buf_.Front();
    if (first_block->Size() > ind) {
        return (*first_block)[ind];
    } else {
        ind -= first_block->Size();
        size_t pos = 1;
        pos += ind / Block::kBlockSize;
        ind -= (pos - 1) * Block::kBlockSize;

        return buf_[pos][ind];
    }
}

int Deque::operator[](size_t ind) const {
    const Block &first_block = buf_.Front();
    if (first_block.Size() > ind) {
        return first_block[ind];
    } else {
        ind -= first_block.Size();
        size_t pos = 1;
        pos += ind / Block::kBlockSize;
        ind -= (pos - 1) * Block::kBlockSize;

        return buf_[pos][ind];
    }
}

size_t Deque::Size() const {
    return size_;
}

void Deque::Clear() {
    while (!buf_.IsEmpty()) {
        buf_.Back()->Clear();
        buf_.PopBack();
    }
    size_ = 0;
}
}  // namespace IntDeque
