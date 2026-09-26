#ifndef STACK_H
#define STACK_H

#include <cstddef>
#include <stdexcept>

// A simple generic Stack ADT implemented with a dynamically
// allocated array. This is a hand-rolled implementation and does
// NOT use std::stack (or any other STL container) internally.
//
// LIFO behavior:
//   push() adds an element to the top of the stack.
//   pop()  removes and returns the element at the top of the stack.
//   top()  returns a reference to the element at the top without
//          removing it.
template <typename T>
class Stack
{
public:
    // Creates an empty stack with a small initial capacity.
    Stack()
        : data_(nullptr), capacity_(0), size_(0)
    {
        reserve(4);
    }

    // Deep-copy constructor / assignment so the class behaves
    // correctly if it is ever copied.
    Stack(const Stack& other)
        : data_(nullptr), capacity_(0), size_(0)
    {
        reserve(other.capacity_);
        for (size_t i = 0; i < other.size_; ++i)
        {
            data_[i] = other.data_[i];
        }
        size_ = other.size_;
    }

    Stack& operator=(const Stack& other)
    {
        if (this != &other)
        {
            delete[] data_;
            data_ = nullptr;
            capacity_ = 0;
            size_ = 0;
            reserve(other.capacity_);
            for (size_t i = 0; i < other.size_; ++i)
            {
                data_[i] = other.data_[i];
            }
            size_ = other.size_;
        }
        return *this;
    }

    ~Stack()
    {
        delete[] data_;
    }

    // Pushes a new value onto the top of the stack. Amortized O(1).
    void push(const T& value)
    {
        if (size_ == capacity_)
        {
            reserve(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        data_[size_] = value;
        ++size_;
    }

    // Removes and returns the top element of the stack. O(1).
    // Throws std::out_of_range if the stack is empty.
    T pop()
    {
        if (empty())
        {
            throw std::out_of_range("Stack::pop(): stack is empty");
        }
        --size_;
        return data_[size_];
    }

    // Returns a reference to the top element without removing it.
    // Throws std::out_of_range if the stack is empty.
    T& top()
    {
        if (empty())
        {
            throw std::out_of_range("Stack::top(): stack is empty");
        }
        return data_[size_ - 1];
    }

    // True if the stack currently holds no elements.
    bool empty() const
    {
        return size_ == 0;
    }

    // Number of elements currently stored in the stack.
    size_t size() const
    {
        return size_;
    }

private:
    T* data_;
    size_t capacity_;
    size_t size_;

    // Grows the backing array to at least newCapacity, copying
    // existing elements over. O(n) when it happens, but doubling
    // the capacity each time keeps push() amortized O(1).
    void reserve(size_t newCapacity)
    {
        if (newCapacity <= capacity_)
        {
            return;
        }

        T* newData = new T[newCapacity];
        for (size_t i = 0; i < size_; ++i)
        {
            newData[i] = data_[i];
        }

        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
    }
};

#endif // STACK_H
