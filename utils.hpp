#pragma once

#include <iostream>

template<typename Container>
void print_container(const Container& container) {
    std::cout << "{ ";

    bool first = true;
    for (const auto& value : container) {
        if (!first) {
            std::cout << ", ";
        }

        std::cout << value;
        first = false;
    }

    std::cout << " }\n";
}
