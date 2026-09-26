#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <cstddef>
#include "Stack.h"

// A fixed-size memory pool (simplified allocator) intended to model
// the kind of buffer pool used for network packet processing.
//
// The pool reserves blockCount * blockSize bytes up front. Free
// blocks are tracked using a custom Stack<void*>, so allocation and
// deallocation are both O(1) and reused memory follows LIFO order.
class MemoryPool
{
public:
    MemoryPool(size_t blockSize, size_t blockCount);
    ~MemoryPool();

    // Not copyable: the pool owns a single raw memory buffer and a
    // set of pointers into it, so copying would be ambiguous/unsafe.
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // Pops a free block off the stack and returns it, or nullptr if
    // no blocks are currently available. O(1).
    void* allocate();

    // Validates ptr, and if it is a currently-allocated block that
    // belongs to this pool, pushes it back onto the free stack and
    // returns true. Returns false for invalid pointers, pointers
    // that don't belong to this pool, or blocks that are already
    // free (double deallocation). O(1).
    bool deallocate(void* ptr);

    size_t availableBlocks() const;
    size_t allocatedBlocks() const;
    size_t blockSize() const;
    size_t capacity() const;

private:
    unsigned char* poolMemory_;  // raw backing storage for all blocks
    size_t blockSize_;
    size_t blockCount_;
    Stack<void*> freeStack_;     // stack of currently-free block pointers
    bool* allocatedFlags_;       // allocatedFlags_[i] == true if block i is in use

    // Returns the block index for ptr if ptr is a valid, block-aligned
    // address within this pool's memory; otherwise returns
    // blockCount_ (an out-of-range sentinel) to signal "invalid".
    size_t indexOf(void* ptr) const;
};

#endif // MEMORY_POOL_H
