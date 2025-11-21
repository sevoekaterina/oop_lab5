#pragma once
#include <memory_resource>
#include <list>
#include <cstddef>

class FixedListMemoryResource : public std::pmr::memory_resource {
private:
    struct Block {
        void* ptr;
        std::size_t size;
        bool is_allocated;
    };

    void* const _buffer;
    const std::size_t _total_size;
    std::list<Block> _blocks;

    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

public:
    explicit FixedListMemoryResource(std::size_t size);
    ~FixedListMemoryResource() override;

    // Запрещаем копирование и перемещение
    FixedListMemoryResource(const FixedListMemoryResource&) = delete;
    FixedListMemoryResource& operator=(const FixedListMemoryResource&) = delete;
};