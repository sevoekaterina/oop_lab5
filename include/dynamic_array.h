#pragma once
#include <memory_resource>
#include <iterator>
#include <stdexcept>

template<typename T>
class DynamicArrayIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

private:
    pointer _ptr;

public:
    explicit DynamicArrayIterator(pointer ptr) : _ptr(ptr) {}

    reference operator*() const { return *_ptr; }
    pointer operator->() const { return _ptr; }

    // Префиксный инкремент
    DynamicArrayIterator& operator++() {
        ++_ptr;
        return *this;
    }

    // Постфиксный инкремент
    DynamicArrayIterator operator++(int) {
        DynamicArrayIterator tmp = *this;
        ++_ptr;
        return tmp;
    }

    bool operator==(const DynamicArrayIterator& other) const {
        return _ptr == other._ptr;
    }

    bool operator!=(const DynamicArrayIterator& other) const {
        return _ptr != other._ptr;
    }
};

template<typename T, typename Allocator = std::pmr::polymorphic_allocator<T>>
class DynamicArray {
private:
    Allocator _allocator;
    T* _data;
    std::size_t _size;
    std::size_t _capacity;

    void resize(std::size_t new_capacity) {
        T* new_data = _allocator.allocate(new_capacity);
        
        for (std::size_t i = 0; i < _size; ++i) {
            try {
                std::construct_at(new_data + i, std::move(_data[i]));
            } catch (...) {
                for (std::size_t j = 0; j < i; ++j) {
                    std::destroy_at(new_data + j);
                }
                _allocator.deallocate(new_data, new_capacity);
                throw;
            }
        }
        
        for (std::size_t i = 0; i < _size; ++i) {
            std::destroy_at(_data + i);
        }
        
        if (_data) {
            _allocator.deallocate(_data, _capacity);
        }
        
        _data = new_data;
        _capacity = new_capacity;
    }

public:
    using value_type = T;
    using allocator_type = Allocator;
    using iterator = DynamicArrayIterator<T>;
    using const_iterator = DynamicArrayIterator<const T>;

    explicit DynamicArray(const Allocator& alloc = Allocator{})
        : _allocator(alloc), _data(nullptr), _size(0), _capacity(0) {}

    DynamicArray(std::size_t initial_size, const Allocator& alloc = Allocator{})
        : _allocator(alloc), _data(nullptr), _size(initial_size), _capacity(initial_size) {
        if (initial_size > 0) {
            _data = _allocator.allocate(initial_size);
            for (std::size_t i = 0; i < initial_size; ++i) {
                std::construct_at(_data + i);
            }
        }
    }

    ~DynamicArray() {
        clear();
        if (_data) {
            _allocator.deallocate(_data, _capacity);
        }
    }

    // Запрещаем копирование (для простоты)
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    // Перемещение
    DynamicArray(DynamicArray&& other) noexcept
        : _allocator(std::move(other._allocator))
        , _data(other._data)
        , _size(other._size)
        , _capacity(other._capacity) {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            clear();
            if (_data) {
                _allocator.deallocate(_data, _capacity);
            }
            
            _allocator = std::move(other._allocator);
            _data = other._data;
            _size = other._size;
            _capacity = other._capacity;
            
            other._data = nullptr;
            other._size = 0;
            other._capacity = 0;
        }
        return *this;
    }

    void push_back(const T& value) {
        if (_size >= _capacity) {
            resize(_capacity == 0 ? 1 : _capacity * 2);
        }
        std::construct_at(_data + _size, value);
        ++_size;
    }

    void push_back(T&& value) {
        if (_size >= _capacity) {
            resize(_capacity == 0 ? 1 : _capacity * 2);
        }
        std::construct_at(_data + _size, std::move(value));
        ++_size;
    }

    void pop_back() {
        if (_size > 0) {
            --_size;
            std::destroy_at(_data + _size);
        }
    }

    T& operator[](std::size_t index) {
        return _data[index];
    }

    const T& operator[](std::size_t index) const {
        return _data[index];
    }

    T& at(std::size_t index) {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }

    const T& at(std::size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }

    std::size_t size() const { return _size; }
    std::size_t capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }

    void clear() {
        for (std::size_t i = 0; i < _size; ++i) {
            std::destroy_at(_data + i);
        }
        _size = 0;
    }

    // Итераторы
    iterator begin() { return iterator(_data); }
    iterator end() { return iterator(_data + _size); }
    const_iterator begin() const { return const_iterator(_data); }
    const_iterator end() const { return const_iterator(_data + _size); }
    const_iterator cbegin() const { return const_iterator(_data); }
    const_iterator cend() const { return const_iterator(_data + _size); }
};