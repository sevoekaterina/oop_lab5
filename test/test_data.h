#pragma once
#include "dynamic_array.h"
#include "memory_resource_fixed_list.h"
#include <cassert>

void test_simple_types() {
    FixedListMemoryResource resource(1024);
    DynamicArray<int, std::pmr::polymorphic_allocator<int>> array(
        std::pmr::polymorphic_allocator<int>(&resource)
    );

    // Тест добавления
    for (int i = 0; i < 100; ++i) {
        array.push_back(i);
    }
    assert(array.size() == 100);

    // Тест итераторов
    int sum = 0;
    for (const auto& elem : array) {
        sum += elem;
    }
    assert(sum == 4950); // 0+1+2+...+99 = 4950

    // Тест доступа
    assert(array[0] == 0);
    assert(array[99] == 99);

    std::cout << "Simple types test passed!" << std::endl;
}

void test_complex_types() {
    FixedListMemoryResource resource(2048);
    
    struct TestStruct {
        int a;
        double b;
        char c;
        
        TestStruct(int x, double y, char z) : a(x), b(y), c(z) {}
    };
    
    DynamicArray<TestStruct, std::pmr::polymorphic_allocator<TestStruct>> array(
        std::pmr::polymorphic_allocator<TestStruct>(&resource)
    );

    // Тест с сложными типами
    for (int i = 0; i < 10; ++i) {
        array.push_back(TestStruct{i, i * 1.5, static_cast<char>('A' + i)});
    }

    assert(array.size() == 10);
    assert(array[0].a == 0);
    assert(array[9].c == 'J');

    std::cout << "Complex types test passed!" << std::endl;
}