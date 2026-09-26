# CIS-277 Assignment 1: Network Packet Buffer Pool

## Student

Benjamin Ruffin

## Description

This project implements a simplified fixed-size memory pool for reusable
network packet buffers. A hand-written generic `Stack<T>` ADT is used to
track which fixed-size blocks in the pool are currently free. Allocating
a block pops a pointer off the stack; releasing a block pushes the
pointer back on. Because the same recently-released block is the next
one handed out, the pool demonstrates memory reuse instead of repeated
system allocation.

`main.cpp` creates an 8-block, 512-byte-per-block pool, allocates and releases blocks, writes and reads back binary
packet data, demonstrates reuse of a released block, exhausts the pool
to show `allocate()` returning `nullptr`, and shows that a block cannot
be deallocated twice.

## Stack Implementation

**Dynamic Array**

I chose a dynamically allocated array (rather than a fixed-size array or
a linked structure) because it gives O(1) amortized `push`/`pop` with
good cache locality and simple bookkeeping, without the per-node allocation overhead of a linked list. The
array doubles in capacity when full, so the stack can grow as needed
while keeping operations O(1) amortized. `std::stack` is not used
anywhere — the underlying storage and all stack operations are
implemented directly in `Stack.h`.

## How to Compile

```
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp MemoryPool.cpp -o buffer_pool
```

## How to Run

```
./buffer_pool
```

## Analysis Questions

**1. Why is a Stack appropriate for managing the free blocks in this
memory pool?**

A Stack's LIFO behavior is a natural fit because the most recently
released block is placed on top and is the next one handed out. This
keeps both `push`  and `pop`  at O(1), with no
need to search for a free block, and it naturally favors reusing
"warm" blocks that were just freed rather than cycling through every
block in the pool.

**2. What happens when the free-block Stack becomes empty?**

When the stack is empty, there are no free blocks left to hand out.
`allocate()` checks `freeStack_.empty()` before popping and, if it is
empty, immediately returns `nullptr` instead of popping. The pool is considered exhausted
until at least one block is deallocated and pushed back onto the
stack.

**3. Why must a released block be returned to the Stack?**

If a released block were not pushed back onto the free stack, it would
become unreachable by `allocate()` — the pool would "lose" that block
permanently even though the memory is still valid and owned by the
pool. Returning it to the stack is what actually makes the memory
available for reuse, which is the entire point of pooling memory
instead of using `new`/`delete` for every packet.

**4. What problem could occur if the same block were deallocated
twice?**

If a block could be pushed onto the free stack twice, the same pointer
would appear on the stack more than once. Two subsequent calls to
`allocate()` would then both return the *same* address as if they were
two independent blocks, even though only one block of memory actually
exists there. Two unrelated parts of the program would then read and
write the same memory believing they each owned it exclusively,
corrupting each other's data. This project prevents it by tracking an
`allocatedFlags_` array and rejecting a `deallocate()` call on a block
that is not currently marked as allocated.

**5. What is the Big-O time complexity of allocate()? Explain why.**

O(1) amortized. `allocate()` only checks whether the stack is empty
and, if not, pops the top element. `pop()` just decrements a size
counter and returns the element at that index — no searching or
shifting of other elements is required, so the cost does not depend on
how many blocks are in the pool.

**6. What is the Big-O time complexity of deallocate()? Explain why.**

O(1) amortized. `deallocate()` computes the block's index from its
pointer with simple pointer arithmetic (subtraction and division —
constant time), checks/updates one entry in the `allocatedFlags_`
array, and pushes the pointer onto the stack. `push()` is O(1)
amortized.  None of these steps depend on the number of blocks currently free or
allocated.
