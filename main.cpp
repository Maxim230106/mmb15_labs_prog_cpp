#include <iostream>
#include <map>
#include <memory>
#include <utility>

#include "pool_allocator.hpp"
#include "singly_linked_list.hpp"

namespace {

int factorial(int value) {
    int result = 1;

    for (int number = 2; number <= value; ++number) {
        result *= number;
    }

    return result;
}

template<typename Map>
void fill_factorial_map(Map& map) {
    for (int key = 0; key < 10; ++key) {
        map[key] = factorial(key);
    }
}

template<typename Map>
void print_map(const Map& map, const char* title) {
    std::cout << title << '\n';

    for (const auto& entry : map) {
        std::cout << entry.first << ' ' << entry.second << '\n';
    }

    std::cout << '\n';
}

template<typename Container>
void fill_sequence(Container& container) {
    for (int value = 0; value < 10; ++value) {
        container.push_back(value);
    }
}

template<typename Container>
void print_sequence(const Container& container, const char* title) {
    std::cout << title << '\n';

    for (const auto& value : container) {
        std::cout << value << '\n';
    }

    std::cout << '\n';
}

} // namespace

int main() {
    std::map<int, int> default_map;
    fill_factorial_map(default_map);
    print_map(default_map, "std::map with std::allocator");

    using MapValue = std::pair<const int, int>;
    using MapAllocator = PoolAllocator<MapValue, 10>;

    std::map<int, int, std::less<int>, MapAllocator> pooled_map;
    fill_factorial_map(pooled_map);
    print_map(pooled_map, "std::map with PoolAllocator<..., 10>");

    SinglyLinkedList<int> default_list;
    fill_sequence(default_list);
    print_sequence(default_list, "SinglyLinkedList with std::allocator");

    SinglyLinkedList<int, PoolAllocator<int, 10>> pooled_list;
    fill_sequence(pooled_list);
    print_sequence(pooled_list, "SinglyLinkedList with PoolAllocator<int, 10>");

    return 0;
}
