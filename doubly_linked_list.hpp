#pragma once

#include <cstddef>   // std::size_t
#include <stdexcept> // std::out_of_range
#include <utility>   // std::move

template<typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;     // Значение, хранящееся в узле списка
        Node* prev; // Указатель на предыдущий узел
        Node* next; // Указатель на следующий узел

        Node(const T& value)
            : data(value), prev(nullptr), next(nullptr) {
        }

        Node(T&& value)
            : data(std::move(value)), prev(nullptr), next(nullptr) {
        }
    };

    Node* head_;       // Первый узел списка
    Node* tail_;       // Последний узел списка
    std::size_t size_; // Количество элементов в списке

    // Освобождает все узлы и переводит список в пустое состояние.
    void clear();
    // Возвращает указатель на узел по индексу.
    Node* node_at(std::size_t index) const;

public:
    class Iterator {
    private:
        Node* current_; // Узел, на который сейчас указывает итератор

    public:
        explicit Iterator(Node* node)
            : current_(node) {
        }

        T& operator*() const {
            return current_->data;
        }

        Iterator& operator++() {
            current_ = current_->next;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current_ != other.current_;
        }

        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }
    };

    class ConstIterator {
    private:
        const Node* current_; // Узел, на который сейчас указывает константный итератор

    public:
        explicit ConstIterator(const Node* node)
            : current_(node) {
        }

        const T& operator*() const {
            return current_->data;
        }

        ConstIterator& operator++() {
            current_ = current_->next;
            return *this;
        }

        bool operator!=(const ConstIterator& other) const {
            return current_ != other.current_;
        }

        bool operator==(const ConstIterator& other) const {
            return current_ == other.current_;
        }
    };

    // Создает пустой список
    DoublyLinkedList();
    // Создает независимую копию другого списка
    DoublyLinkedList(const DoublyLinkedList& other);
    // Забирает узлы у временного списка
    DoublyLinkedList(DoublyLinkedList&& other) noexcept;

    // Полностью заменяет содержимое текущего списка копией other
    DoublyLinkedList& operator=(const DoublyLinkedList& other);
    // Передает владение узлами от временного списка текущему
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept;

    // Освобождает все узлы списка
    ~DoublyLinkedList();

    // Добавляет элемент в конец списка по копии
    void push_back(const T& value);
    // Добавляет элемент в конец списка с перемещением
    void push_back(T&& value);

    // Вставляет элемент в указанную позицию
    void insert(std::size_t index, const T& value);
    // Вставляет элемент в указанную позицию с перемещением
    void insert(std::size_t index, T&& value);
    // Удаляет элемент по индексу
    void erase(std::size_t index);

    // Возвращает число элементов в списке
    std::size_t size() const;
    // Проверяет, пуст ли список
    bool empty() const;

    // Возвращает ссылку на элемент по индексу
    T& operator[](std::size_t index);
    // Константная версия доступа по индексу
    const T& operator[](std::size_t index) const;

    // Итераторы для обхода списка
    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;
};

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList()
    : head_(nullptr), tail_(nullptr), size_(0) {
}

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList& other)
    : head_(nullptr), tail_(nullptr), size_(0) {
    for (const T& value : other) {
        push_back(value);
    }
}

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList(DoublyLinkedList&& other) noexcept
    : head_(other.head_), tail_(other.tail_), size_(other.size_) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
}

template<typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList& other) {
    if (this == &other) {
        return *this;
    }

    DoublyLinkedList copy(other);
    *this = std::move(copy);

    return *this;
}

template<typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(DoublyLinkedList&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    clear();

    head_ = other.head_;
    tail_ = other.tail_;
    size_ = other.size_;

    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;

    return *this;
}

template<typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    clear();
}

template<typename T>
void DoublyLinkedList<T>::clear() {
    Node* current = head_;

    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }

    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
}

template<typename T>
typename DoublyLinkedList<T>::Node* DoublyLinkedList<T>::node_at(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("DoublyLinkedList index is out of range");
    }

    if (index < size_ / 2) {
        Node* current = head_;

        for (std::size_t current_index = 0; current_index < index; ++current_index) {
            current = current->next;
        }

        return current;
    }

    Node* current = tail_;

    for (std::size_t current_index = size_ - 1; current_index > index; --current_index) {
        current = current->prev;
    }

    return current;
}

template<typename T>
void DoublyLinkedList<T>::push_back(const T& value) {
    Node* new_node = new Node(value);

    if (tail_ == nullptr) {
        head_ = new_node;
        tail_ = new_node;
    } else {
        tail_->next = new_node;
        new_node->prev = tail_;
        tail_ = new_node;
    }

    ++size_;
}

template<typename T>
void DoublyLinkedList<T>::push_back(T&& value) {
    Node* new_node = new Node(std::move(value));

    if (tail_ == nullptr) {
        head_ = new_node;
        tail_ = new_node;
    } else {
        tail_->next = new_node;
        new_node->prev = tail_;
        tail_ = new_node;
    }

    ++size_;
}

template<typename T>
void DoublyLinkedList<T>::insert(std::size_t index, const T& value) {
    if (index > size_) {
        throw std::out_of_range("DoublyLinkedList insert index is out of range");
    }

    if (index == size_) {
        push_back(value);
        return;
    }

    Node* new_node = new Node(value);
    Node* next_node = node_at(index);
    Node* prev_node = next_node->prev;

    new_node->next = next_node;
    new_node->prev = prev_node;
    next_node->prev = new_node;

    if (prev_node != nullptr) {
        prev_node->next = new_node;
    } else {
        head_ = new_node;
    }

    ++size_;
}

template<typename T>
void DoublyLinkedList<T>::insert(std::size_t index, T&& value) {
    if (index > size_) {
        throw std::out_of_range("DoublyLinkedList insert index is out of range");
    }

    if (index == size_) {
        push_back(std::move(value));
        return;
    }

    Node* new_node = new Node(std::move(value));
    Node* next_node = node_at(index);
    Node* prev_node = next_node->prev;

    new_node->next = next_node;
    new_node->prev = prev_node;
    next_node->prev = new_node;

    if (prev_node != nullptr) {
        prev_node->next = new_node;
    } else {
        head_ = new_node;
    }

    ++size_;
}

template<typename T>
void DoublyLinkedList<T>::erase(std::size_t index) {
    Node* target = node_at(index);
    Node* prev_node = target->prev;
    Node* next_node = target->next;

    if (prev_node != nullptr) {
        prev_node->next = next_node;
    } else {
        head_ = next_node;
    }

    if (next_node != nullptr) {
        next_node->prev = prev_node;
    } else {
        tail_ = prev_node;
    }

    delete target;
    --size_;
}

template<typename T>
std::size_t DoublyLinkedList<T>::size() const {
    return size_;
}

template<typename T>
bool DoublyLinkedList<T>::empty() const {
    return size_ == 0;
}

template<typename T>
T& DoublyLinkedList<T>::operator[](std::size_t index) {
    return node_at(index)->data;
}

template<typename T>
const T& DoublyLinkedList<T>::operator[](std::size_t index) const {
    return node_at(index)->data;
}

template<typename T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::begin() {
    return Iterator(head_);
}

template<typename T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::end() {
    return Iterator(nullptr);
}

template<typename T>
typename DoublyLinkedList<T>::ConstIterator DoublyLinkedList<T>::begin() const {
    return ConstIterator(head_);
}

template<typename T>
typename DoublyLinkedList<T>::ConstIterator DoublyLinkedList<T>::end() const {
    return ConstIterator(nullptr);
}
