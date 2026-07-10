#pragma once

#include <cstddef>   // std::size_t
#include <stdexcept> // std::out_of_range
#include <utility>   // std::move

template<typename T>
class SinglyLinkedList {
private:
    struct Node {
        T data;     // Значение, которое хранит узел
        Node* next; // Указатель на следующий узел

        Node(const T& value)
            : data(value), next(nullptr) {
        }

        Node(T&& value)
            : data(std::move(value)), next(nullptr) {
        }
    };

    Node* head_;       // Первый узел списка
    Node* tail_;       // Последний узел списка для быстрого push_back
    std::size_t size_; // Количество элементов в списке

    // Освобождает все узлы и делает список пустым
    void clear();
    // Возвращает указатель на узел по индексу
    Node* node_at(std::size_t index) const;

public:
    class Iterator {
    private:
        Node* current_; // Узел, на который сейчас смотрит итератор

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

        Node* get() const {
            return current_;
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
        const Node* current_; // Узел, на который смотрит константный итератор

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

        const Node* get() const {
            return current_;
        }

        bool operator!=(const ConstIterator& other) const {
            return current_ != other.current_;
        }

        bool operator==(const ConstIterator& other) const {
            return current_ == other.current_;
        }
    };

    // Создает пустой список
    SinglyLinkedList();
    // Создает независимую копию другого списка
    SinglyLinkedList(const SinglyLinkedList& other);
    // Забирает ресурсы у временного списка
    SinglyLinkedList(SinglyLinkedList&& other) noexcept;

    // Полностью заменяет содержимое текущего списка копией other
    SinglyLinkedList& operator=(const SinglyLinkedList& other);
    // Передает владение узлами от временного списка текущему
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept;

    // Освобождает все узлы списка
    ~SinglyLinkedList();

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

    // Возвращает текущее число элементов
    std::size_t size() const;
    // Проверяет, пуст ли список
    bool empty() const;

    // Доступ к элементу по индексу
    T& operator[](std::size_t index);
    // Константный доступ к элементу по индексу
    const T& operator[](std::size_t index) const;

    // Итераторы для обхода списка
    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;
};

template<typename T>
SinglyLinkedList<T>::SinglyLinkedList()
    : head_(nullptr), tail_(nullptr), size_(0) {
}

template<typename T>
SinglyLinkedList<T>::SinglyLinkedList(const SinglyLinkedList& other)
    : head_(nullptr), tail_(nullptr), size_(0) {
    for (const T& value : other) {
        push_back(value);
    }
}

template<typename T>
SinglyLinkedList<T>::SinglyLinkedList(SinglyLinkedList&& other) noexcept
    : head_(other.head_), tail_(other.tail_), size_(other.size_) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
}

template<typename T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(const SinglyLinkedList& other) {
    if (this == &other) {
        return *this;
    }

    SinglyLinkedList copy(other);
    *this = std::move(copy);

    return *this;
}

template<typename T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(SinglyLinkedList&& other) noexcept {
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
SinglyLinkedList<T>::~SinglyLinkedList() {
    clear();
}

template<typename T>
void SinglyLinkedList<T>::clear() {
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
typename SinglyLinkedList<T>::Node* SinglyLinkedList<T>::node_at(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("SinglyLinkedList index is out of range");
    }

    Node* current = head_;

    for (std::size_t current_index = 0; current_index < index; ++current_index) {
        current = current->next;
    }

    return current;
}

template<typename T>
void SinglyLinkedList<T>::push_back(const T& value) {
    Node* new_node = new Node(value);

    if (tail_ == nullptr) {
        head_ = new_node;
        tail_ = new_node;
    } else {
        tail_->next = new_node;
        tail_ = new_node;
    }

    ++size_;
}

template<typename T>
void SinglyLinkedList<T>::push_back(T&& value) {
    Node* new_node = new Node(std::move(value));

    if (tail_ == nullptr) {
        head_ = new_node;
        tail_ = new_node;
    } else {
        tail_->next = new_node;
        tail_ = new_node;
    }

    ++size_;
}

template<typename T>
void SinglyLinkedList<T>::insert(std::size_t index, const T& value) {
    if (index > size_) {
        throw std::out_of_range("SinglyLinkedList insert index is out of range");
    }

    if (index == size_) {
        push_back(value);
        return;
    }

    Node* new_node = new Node(value);

    if (index == 0) {
        new_node->next = head_;
        head_ = new_node;

        if (tail_ == nullptr) {
            tail_ = new_node;
        }

        ++size_;
        return;
    }

    Node* previous = node_at(index - 1);
    new_node->next = previous->next;
    previous->next = new_node;

    ++size_;
}

template<typename T>
void SinglyLinkedList<T>::insert(std::size_t index, T&& value) {
    if (index > size_) {
        throw std::out_of_range("SinglyLinkedList insert index is out of range");
    }

    if (index == size_) {
        push_back(std::move(value));
        return;
    }

    Node* new_node = new Node(std::move(value));

    if (index == 0) {
        new_node->next = head_;
        head_ = new_node;

        if (tail_ == nullptr) {
            tail_ = new_node;
        }

        ++size_;
        return;
    }

    Node* previous = node_at(index - 1);
    new_node->next = previous->next;
    previous->next = new_node;

    ++size_;
}

template<typename T>
void SinglyLinkedList<T>::erase(std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("SinglyLinkedList erase index is out of range");
    }

    Node* target = nullptr;

    if (index == 0) {
        target = head_;
        head_ = head_->next;

        if (head_ == nullptr) {
            tail_ = nullptr;
        }
    } else {
        Node* previous = node_at(index - 1);
        target = previous->next;
        previous->next = target->next;

        if (target == tail_) {
            tail_ = previous;
        }
    }

    delete target;
    --size_;
}

template<typename T>
std::size_t SinglyLinkedList<T>::size() const {
    return size_;
}

template<typename T>
bool SinglyLinkedList<T>::empty() const {
    return size_ == 0;
}

template<typename T>
T& SinglyLinkedList<T>::operator[](std::size_t index) {
    return node_at(index)->data;
}

template<typename T>
const T& SinglyLinkedList<T>::operator[](std::size_t index) const {
    return node_at(index)->data;
}

template<typename T>
typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::begin() {
    return Iterator(head_);
}

template<typename T>
typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::end() {
    return Iterator(nullptr);
}

template<typename T>
typename SinglyLinkedList<T>::ConstIterator SinglyLinkedList<T>::begin() const {
    return ConstIterator(head_);
}

template<typename T>
typename SinglyLinkedList<T>::ConstIterator SinglyLinkedList<T>::end() const {
    return ConstIterator(nullptr);
}
