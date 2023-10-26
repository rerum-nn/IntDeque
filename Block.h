#pragma once

#include <cstddef>  // size_t
#include <vector>

namespace IntDeque {
namespace IntBlock {
class Block {
public:
    Block();
    Block(const Block &other);
    Block(Block &&other) noexcept;

    Block &operator=(const Block &other);
    Block &operator=(Block &&other) noexcept;

    int &operator[](size_t i);
    int operator[](size_t i) const;

    size_t Size() const;

    void PushBack(int i);
    void PopBack();
    void PushFront(int i);
    void PopFront();

    bool IsFull() const;
    bool IsEmpty() const;

    bool IsDownBorder() const;
    bool IsUpBorder() const;

    void Clear();

    static constexpr const size_t kBlockSize = 128;

private:
    std::vector<int> block_;
    size_t size_;

    size_t backward_pos_;
    size_t forward_pos_;
};
}  // namespace IntBlock
}  // namespace IntDeque
