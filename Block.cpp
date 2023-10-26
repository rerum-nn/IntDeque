#include <cassert>
#include "Block.h"

namespace IntDeque {
namespace IntBlock {
Block::Block() : block_(kBlockSize, 0), size_(0), backward_pos_(kBlockSize), forward_pos_(0) {
}

Block::Block(const Block &other)
    : block_(other.block_),
      size_(other.size_),
      backward_pos_(other.backward_pos_),
      forward_pos_(other.forward_pos_) {
    assert(block_.size() == kBlockSize && "Size of copyable Block is not default");
}

Block::Block(Block &&other) noexcept
    : block_(std::move(other.block_)),
      size_(other.size_),
      backward_pos_(other.backward_pos_),
      forward_pos_(other.forward_pos_) {
    other.size_ = 0;
    other.backward_pos_ = 0;
    other.forward_pos_ = 0;
}

Block &Block::operator=(const Block &other) {
    if (this == &other) {
        return *this;
    }

    block_ = other.block_;
    size_ = other.size_;
    backward_pos_ = other.backward_pos_;
    forward_pos_ = other.forward_pos_;

    return *this;
}

Block &Block::operator=(Block &&other) noexcept {
    block_ = std::move(other.block_);
    size_ = other.size_;
    backward_pos_ = other.backward_pos_;
    forward_pos_ = other.forward_pos_;

    other.size_ = 0;
    other.backward_pos_ = 0;
    other.forward_pos_ = 0;

    return *this;
}

int &Block::operator[](size_t i) {
    assert(i < size_ && "i greater than size of Block");
    return block_[backward_pos_ + i];
}
int Block::operator[](size_t i) const {
    assert(i < size_ && "i greater than size of Block");
    return block_[backward_pos_ + i];
}

size_t Block::Size() const {
    return size_;
}

void Block::PushBack(int i) {
    if (IsEmpty()) {
        backward_pos_ = 0;
    }
    block_[forward_pos_] = i;
    ++forward_pos_;
    ++size_;
}

void Block::PopBack() {
    assert(size_ && "Block is empty, block manipulation in Deque error");
    --forward_pos_;
    --size_;
    if (IsEmpty()) {
        backward_pos_ = kBlockSize - 1;
    }
}

void Block::PushFront(int i) {
    if (IsEmpty()) {
        forward_pos_ = kBlockSize;
    }
    --backward_pos_;
    block_[backward_pos_] = i;
    ++size_;
}

void Block::PopFront() {
    assert(size_ && "Block is empty, block manipulation in Deque error");
    ++backward_pos_;
    --size_;
    if (IsEmpty()) {
        forward_pos_ = 0;
    }
}

bool Block::IsFull() const {
    return size_ == kBlockSize;
}
bool Block::IsEmpty() const {
    return size_ == 0;
}

bool Block::IsDownBorder() const {
    return backward_pos_ == 0;
}
bool Block::IsUpBorder() const {
    return forward_pos_ == kBlockSize;
}

void Block::Clear() {
    size_ = 0;
    backward_pos_ = kBlockSize;
    forward_pos_ = 0;
}
}  // namespace IntBlock
}  // namespace IntDeque
