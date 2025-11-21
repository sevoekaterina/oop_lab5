#include <iostream>
#include <memory_resource>
#include "include/memory_resource_fixed_list.h"
#include "include/dynamic_array.h"

struct Person {
    int id;
    std::string name;
    double salary;

    Person() : id(0), name(""), salary(0.0) {}
    Person(int i, const std::string& n, double s) : id(i), name(n), salary(s) {}

    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        os << "Person{id: " << p.id << ", name: " << p.name << ", salary: " << p.salary << "}";
        return os;
    }
};

void demonstrate_with_ints() {
    std::cout << "=== Demonstration with int ===" << std::endl;
    
    FixedListMemoryResource resource(1024);
    
    // ИСПРАВЛЕНИЕ: Используем uniform initialization с {}
    DynamicArray<int, std::pmr::polymorphic_allocator<int>> intArray{
        std::pmr::polymorphic_allocator<int>(&resource)
    };

    for (int i = 0; i < 10; ++i) {
        intArray.push_back(i * i);
    }

    std::cout << "Array elements using iterator: ";
    for (auto it = intArray.begin(); it != intArray.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Array elements using range-based for: ";
    for (const auto& elem : intArray) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    std::cout << "Size: " << intArray.size() << ", Capacity: " << intArray.capacity() << std::endl;
}

void demonstrate_with_persons() {
    std::cout << "\n=== Demonstration with Person ===" << std::endl;
    
    FixedListMemoryResource resource(2048);
    
    // ИСПРАВЛЕНИЕ: Используем uniform initialization с {}
    DynamicArray<Person, std::pmr::polymorphic_allocator<Person>> people{
        std::pmr::polymorphic_allocator<Person>(&resource)
    };

    people.push_back(Person(1, "Alice", 50000.0));
    people.push_back(Person(2, "Bob", 60000.0));
    people.push_back(Person(3, "Charlie", 70000.0));

    std::cout << "People using iterator:" << std::endl;
    for (auto it = people.begin(); it != people.end(); ++it) {
        std::cout << "  " << *it << std::endl;
    }

    auto it = people.begin();
    ++it;
    it->salary = 65000.0;

    std::cout << "\nAfter modification:" << std::endl;
    for (const auto& person : people) {
        std::cout << "  " << person << std::endl;
    }

    people.pop_back();
    std::cout << "\nAfter pop_back:" << std::endl;
    std::cout << "Size: " << people.size() << std::endl;
    
    for (const auto& person : people) {
        std::cout << "  " << person << std::endl;
    }
}

void demonstrate_memory_reuse() {
    std::cout << "\n=== Demonstration of Memory Reuse ===" << std::endl;
    
    FixedListMemoryResource resource(512);
    
    // ИСПРАВЛЕНИЕ: Используем uniform initialization с {}
    DynamicArray<int, std::pmr::polymorphic_allocator<int>> testArray{
        std::pmr::polymorphic_allocator<int>(&resource)
    };

    std::cout << "Initial state - empty array" << std::endl;

    for (int i = 0; i < 5; ++i) {
        testArray.push_back(i);
    }
    std::cout << "After adding 5 elements: ";
    for (const auto& elem : testArray) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    testArray.pop_back();
    testArray.pop_back();
    std::cout << "After 2 pop_back operations: ";
    for (const auto& elem : testArray) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    testArray.push_back(100);
    testArray.push_back(200);
    std::cout << "After re-adding elements: ";
    for (const auto& elem : testArray) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

int main() {
    try {
        demonstrate_with_ints();
        demonstrate_with_persons();
        demonstrate_memory_reuse();
        
        std::cout << "\nAll demonstrations completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}