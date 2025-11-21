#include "../include/memory_resource_fixed_list.h"
#include <iostream>
#include <cassert>

FixedListMemoryResource::FixedListMemoryResource(std::size_t size)
    : _buffer(::operator new(size))
    , _total_size(size) {
    
    _blocks.push_back({_buffer, size, false});
    std::cout << "FixedListMemoryResource created with " << size << " bytes\n";
}

FixedListMemoryResource::~FixedListMemoryResource() {
    for (const auto& block : _blocks) {
        if (block.is_allocated) {
            std::cout << "WARNING: Memory leak detected - block at " 
                      << block.ptr << " of size " << block.size << " was not deallocated\n";
        }
    }
    ::operator delete(_buffer);
    std::cout << "FixedListMemoryResource destroyed\n";
}

void* FixedListMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    for (auto it = _blocks.begin(); it != _blocks.end(); ++it) {
        if (!it->is_allocated && it->size >= bytes) {
            void* allocated_ptr = it->ptr;
            
            if (it->size > bytes) {
                Block new_free_block = {
                    static_cast<char*>(it->ptr) + bytes,
                    it->size - bytes,
                    false
                };
                
                it->size = bytes;
                it->is_allocated = true;
                _blocks.insert(++it, new_free_block);
                --it;
            } else {
                it->is_allocated = true;
            }
            
            std::cout << "Allocated " << bytes << " bytes at " << allocated_ptr << "\n";
            return allocated_ptr;
        }
    }
    
    throw std::bad_alloc();
}

void FixedListMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
    for (auto it = _blocks.begin(); it != _blocks.end(); ++it) {
        if (it->ptr == p && it->is_allocated) {
            it->is_allocated = false;
            std::cout << "Deallocated block at " << p << " of size " << bytes << "\n";
            
            auto next = it;
            ++next;
            if (next != _blocks.end() && !next->is_allocated) {
                it->size += next->size;
                _blocks.erase(next);
            }
            
            if (it != _blocks.begin()) {
                auto prev = it;
                --prev;
                if (!prev->is_allocated) {
                    prev->size += it->size;
                    _blocks.erase(it);
                }
            }
            
            return;
        }
    }
    
    throw std::runtime_error("Attempt to deallocate invalid pointer");
}

bool FixedListMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}