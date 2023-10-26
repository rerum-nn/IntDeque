#pragma once

#include <initializer_list>
#include <algorithm>
#include <deque>

static const size_t kBlockSize = 128;

template <typename T>
class RingBuffer {
public:
    RingBuffer(size_t capacity = 1) : size_(0), capacity_(capacity), first_elem_(0) {
        begin_ = new T[capacity];
    }

    RingBuffer(const RingBuffer& rb) {
        begin_ = new T[rb.capacity_];
        size_ = rb.size_;
        capacity_ = rb.capacity_;
        first_elem_ = rb.first_elem_;
        for (size_t i = 0; i < rb.size_; ++i) {
            begin_[(first_elem_ + i) % capacity_] = rb.begin_[(first_elem_ + i) % capacity_];
        }
    }

    RingBuffer(RingBuffer&& rb) {
        begin_ = rb.begin_;
        size_ = rb.size_;
        capacity_ = rb.capacity_;
        first_elem_ = rb.first_elem_;

        rb.begin_ = nullptr;
        rb.size_ = 0;
        rb.capacity_ = 0;
        rb.first_elem_ = 0;
    }

    RingBuffer& operator=(const RingBuffer& rb) {
        if (this == &rb) {
            return *this;
        }
        delete[] begin_;

        begin_ = new T[rb.capacity_];
        size_ = rb.size_;
        capacity_ = rb.capacity_;
        first_elem_ = rb.first_elem_;
        for (size_t i = 0; i < rb.size_; ++i) {
            begin_[(first_elem_ + i) % capacity_] = rb.begin_[(first_elem_ + i) % capacity_];
        }

        return *this;
    }

    RingBuffer& operator=(RingBuffer&& rb) {
        delete[] begin_;

        begin_ = rb.begin_;
        size_ = rb.size_;
        capacity_ = rb.capacity_;
        first_elem_ = rb.first_elem_;

        rb.begin_ = nullptr;
        rb.size_ = 0;
        rb.capacity_ = 0;
        rb.first_elem_ = 0;

        return *this;
    }

    ~RingBuffer() {
        delete[] begin_;
    }

    void PushBack(T&& elem) {
        if (size_ == capacity_) {
            Realloc(capacity_ * 2);
        }
        begin_[(first_elem_ + size_) % capacity_] = std::move(elem);
        ++size_;
    }
    void PopBack() {
        if (size_ * 4 == capacity_) {
            Realloc(capacity_ / 2);
        }
        --size_;
    }

    void PushFront(T&& elem) {
        if (size_ == capacity_) {
            Realloc(capacity_ * 2);
        }
        first_elem_ = (first_elem_ - 1) % capacity_;
        begin_[first_elem_] = std::move(elem);
        ++size_;
    }
    void PopFront() {
        if (size_ * 4 == capacity_) {
            Realloc(capacity_ / 2);
        }
        first_elem_ = (first_elem_ + 1) % capacity_;
        --size_;
    }

    T* Back() {
        return &begin_[(first_elem_ + size_ - 1) % capacity_];
    }

    T Back() const {
        return begin_[(first_elem_ + size_ - 1) % capacity_];
    }

    T* Front() {
        return &begin_[first_elem_];
    }

    T Front() const {
        return begin_[first_elem_];
    }

    T* operator[](size_t i) {
        return &begin_[(first_elem_ + i) % capacity_];
    }

    T operator[](size_t i) const {
        return begin_[(first_elem_ + i) % capacity_];
    }

    bool IsEmpty() const {
        return size_ == 0;
    }

private:
    T* begin_;
    size_t size_;
    size_t capacity_;

    size_t first_elem_;

    void Realloc(size_t capacity) {
        T* old_begin = begin_;
        begin_ = new T[capacity];
        for (size_t i = 0; i < size_; ++i) {
            begin_[i] = std::move(old_begin[(first_elem_ + i) % capacity_]);
        }
        delete[] old_begin;
        first_elem_ = 0;
        capacity_ = capacity;
    }
};

class Block {

public:
    Block() : size_(0), backward_pos_(kBlockSize), forward_pos_(0) {
        block_ = new int[kBlockSize];
    }

    Block(const Block& other) {
        size_ = other.size_;
        backward_pos_ = other.backward_pos_;
        forward_pos_ = other.forward_pos_;
        block_ = new int[kBlockSize];
        for (size_t i = 0; i < kBlockSize; ++i) {
            block_[i] = other.block_[i];
        }
    }

    Block(Block&& other) {
        size_ = other.size_;
        backward_pos_ = other.backward_pos_;
        forward_pos_ = other.forward_pos_;
        block_ = other.block_;
        other.block_ = nullptr;
        other.size_ = 0;
        other.backward_pos_ = 0;
        other.forward_pos_ = 0;
    }

    Block& operator=(const Block& other) {
        if (this == &other) {
            return *this;
        }

        size_ = other.size_;
        backward_pos_ = other.backward_pos_;
        forward_pos_ = other.forward_pos_;

        delete[] block_;
        block_ = new int[kBlockSize];
        for (size_t i = 0; i < kBlockSize; ++i) {
            block_[i] = other.block_[i];
        }

        return *this;
    }

    Block& operator=(Block&& other) {
        delete[] block_;
        block_ = other.block_;
        size_ = other.size_;
        backward_pos_ = other.backward_pos_;
        forward_pos_ = other.forward_pos_;

        other.block_ = nullptr;
        other.size_ = 0;
        other.backward_pos_ = 0;
        other.forward_pos_ = 0;

        return *this;
    }

    int& operator[](size_t i) {
        return block_[backward_pos_ + i];
    }

    int operator[](size_t i) const {
        return block_[backward_pos_ + i];
    }

    size_t Size() const {
        return size_;
    }

    void PushForward(int i) {
        if (IsEmpty()) {
            backward_pos_ = 0;
        }
        block_[forward_pos_] = i;
        ++forward_pos_;
        ++size_;
    }

    void PopForward() {
        --forward_pos_;
        --size_;
        if (IsEmpty()) {
            backward_pos_ = kBlockSize - 1;
        }
    }

    void PushBackward(int i) {
        if (IsEmpty()) {
            forward_pos_ = kBlockSize;
        }
        --backward_pos_;
        block_[backward_pos_] = i;
        ++size_;
    }

    void PopBackward() {
        ++backward_pos_;
        --size_;
        if (IsEmpty()) {
            forward_pos_ = 0;
        }
    }

    bool IsFull() {
        return size_ == kBlockSize;
    }

    bool IsEmpty() {
        return size_ == 0;
    }

    bool IsDownBorder() {
        return backward_pos_ == 0;
    }

    bool IsUpBorder() {
        return forward_pos_ == kBlockSize;
    }

    void Clear() {
        size_ = 0;
        backward_pos_ = kBlockSize;
        forward_pos_ = 0;
    }

    ~Block() {
        delete[] block_;
    }

private:
    int* block_;
    size_t size_;

    size_t backward_pos_;
    size_t forward_pos_;
};

class Deque {
public:
    Deque() : size_(0) {
        buf_.PushBack(Block());
    }
    Deque(const Deque& rhs) = default;
    Deque(Deque&& rhs) = default;
    explicit Deque(size_t size) : size_(size) {
        for (size_t i = 0; i < size; i += kBlockSize) {
            Block b;
            for (size_t j = 0; j < kBlockSize; j++) {
                b.PushForward(0);
            }
            buf_.PushBack(std::move(b));
        }
    }

    Deque(std::initializer_list<int> list) {
        for (size_t i = 0; i < list.size(); i += kBlockSize) {
            Block b;
            for (size_t j = 0; i + j < list.size() && j < kBlockSize; ++j) {
                b.PushForward(*(list.begin() + i + j));
            }
            buf_.PushBack(std::move(b));
        }
        size_ = list.size();
    }

    Deque& operator=(Deque rhs) {
        Swap(rhs);
        return *this;
    }

    void Swap(Deque& rhs) {
        std::swap(buf_, rhs.buf_);
        std::swap(size_, rhs.size_);
    }

    void PushBack(int value) {
        Block* last_block = buf_.Back();
        if (last_block->IsUpBorder()) {
            buf_.PushBack(Block());
            last_block = buf_.Back();
        }
        last_block->PushForward(value);
        ++size_;
    }

    void PopBack() {
        Block* last_block = buf_.Back();
        if (last_block->IsEmpty()) {
            buf_.PopBack();
            last_block = buf_.Back();
        }
        last_block->PopForward();
        --size_;
    }

    void PushFront(int value) {
        Block* first_block = buf_.Front();
        if (first_block->IsDownBorder()) {
            buf_.PushFront(Block());
            first_block = buf_.Front();
        }
        first_block->PushBackward(value);
        ++size_;
    }

    void PopFront() {
        Block* first_block = buf_.Front();
        if (first_block->IsEmpty()) {
            buf_.PopFront();
            first_block = buf_.Front();
        }
        first_block->PopBackward();
        --size_;
    }

    int& operator[](size_t ind) {
        Block* first_block = buf_.Front();
        if (first_block->Size() > ind) {
            return (*first_block)[ind];
        } else {
            ind -= first_block->Size();
            size_t pos = 1;
            pos += ind / kBlockSize;
            ind -= (pos - 1) * kBlockSize;

            return (*buf_[pos])[ind];
        }
    }

    int operator[](size_t ind) const {
        Block first_block = buf_.Front();
        if (first_block.Size() > ind) {
            return first_block[ind];
        } else {
            ind -= first_block.Size();
            size_t pos = 1;
            pos += ind / kBlockSize;
            ind -= (pos - 1) * kBlockSize;

            return buf_[pos][ind];
        }
    }

    size_t Size() const {
        return size_;
    }

    void Clear() {
        while (!buf_.IsEmpty()) {
            buf_.Back()->Clear();
            buf_.PopBack();
        }
        size_ = 0;
    }

private:
    RingBuffer<Block> buf_;
    size_t size_ = 0;
};