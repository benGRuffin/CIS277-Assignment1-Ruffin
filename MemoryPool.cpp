#include "MemoryPool.h"

MemoryPool::MemoryPool(size_t blockSize, size_t blockCount)
    : poolMemory_(nullptr),
      blockSize_(blockSize),
      blockCount_(blockCount),
      freeStack_(),
      allocatedFlags_(nullptr)
{
    // Reserve one contiguous buffer big enough to hold every block.
    poolMemory_ = new unsigned char[blockSize_ * blockCount_];
    allocatedFlags_ = new bool[blockCount_];

    // Push every block onto the free stack so all blocks start out
    // available. Pushing in order 0, 1, 2, ... means block
    // (blockCount_ - 1) ends up on top, matching the assignment's
    // "Block 7 on top" example for an 8-block pool.
    for (size_t i = 0; i < blockCount_; ++i)
    {
        allocatedFlags_[i] = false;
        void* blockPtr = static_cast<void*>(poolMemory_ + (i * blockSize_));
        freeStack_.push(blockPtr);
    }
}

MemoryPool::~MemoryPool()
{
    delete[] poolMemory_;
    delete[] allocatedFlags_;
}

void* MemoryPool::allocate()
{
    if (freeStack_.empty())
    {
        return nullptr;
    }

    void* blockPtr = freeStack_.pop();
    size_t index = indexOf(blockPtr);
    // Every pointer that was ever pushed came from this pool, so
    // this should always resolve to a valid index.
    allocatedFlags_[index] = true;
    return blockPtr;
}

bool MemoryPool::deallocate(void* ptr)
{
    size_t index = indexOf(ptr);

    // Pointer does not belong to this pool / is not block-aligned.
    if (index >= blockCount_)
    {
        return false;
    }

    // Block is not currently allocated: either it was never
    // allocated, or this is a double deallocation. Reject it so we
    // never push an invalid/duplicate block onto the free stack.
    if (!allocatedFlags_[index])
    {
        return false;
    }

    allocatedFlags_[index] = false;
    freeStack_.push(ptr);
    return true;
}

size_t MemoryPool::availableBlocks() const
{
    return freeStack_.size();
}

size_t MemoryPool::allocatedBlocks() const
{
    return blockCount_ - freeStack_.size();
}

size_t MemoryPool::blockSize() const
{
    return blockSize_;
}

size_t MemoryPool::capacity() const
{
    return blockSize_ * blockCount_;
}

size_t MemoryPool::indexOf(void* ptr) const
{
    unsigned char* bytePtr = static_cast<unsigned char*>(ptr);

    // Reject null and anything outside the pool's memory range.
    if (bytePtr == nullptr ||
        bytePtr < poolMemory_ ||
        bytePtr >= poolMemory_ + (blockSize_ * blockCount_))
    {
        return blockCount_; // sentinel: invalid
    }

    size_t offset = static_cast<size_t>(bytePtr - poolMemory_);

    // Reject pointers that don't land exactly on a block boundary
    // (e.g. someone passing a pointer into the middle of a block).
    if (offset % blockSize_ != 0)
    {
        return blockCount_; // sentinel: invalid
    }

    return offset / blockSize_;
}
