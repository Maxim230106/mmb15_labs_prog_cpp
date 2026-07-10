#include <iostream>

#include "dynamic_array.hpp"      // последовательный
#include "doubly_linked_list.hpp" // списковый двухсторонний
#include "singly_linked_list.hpp" // списковый односторонний
#include "utils.hpp"              // дополнительные инструменты (вывод контейнеров)

template<typename Container>
void run_demo(Container& container, const char* container_name) {
    for (int value = 0; value < 10; ++value) {
        container.push_back(value);
    }

    std::cout << container_name << " initial: ";
    print_container(container);

    std::cout << "Size: " << container.size() << '\n';

    // Удаляем в обратном порядке, чтобы индексы оставшихся элементов не съехали
    container.erase(6);
    container.erase(4);
    container.erase(2);

    std::cout << container_name << " after erase: ";
    print_container(container);

    container.insert(0, 10);
    std::cout << container_name << " after insert at begin: ";
    print_container(container);

    container.insert(container.size() / 2, 20);
    std::cout << container_name << " after insert in middle: ";
    print_container(container);

    container.push_back(30);
    std::cout << container_name << " after insert at end: ";
    print_container(container);

    std::cout << '\n';
}

int main() {
    DynamicArray<int> array;
    run_demo(array, "DynamicArray");

    DoublyLinkedList<int> list_d;
    run_demo(list_d, "DoublyLinkedList");

    SinglyLinkedList<int> list_s;
    run_demo(list_s, "SinglyLinkedList");

    return 0;
}
