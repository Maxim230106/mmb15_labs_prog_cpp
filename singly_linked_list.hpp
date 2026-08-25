#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<typename T, typename Alloc = std::allocator<T>>
class SinglyLinkedList {
private:
    struct Node {
        T data;
        Node* next;

        template<typename U>
        explicit Node(U&& value)
            : data(std::forward<U>(value)), next(nullptr) {
        }
    };

    using NodeAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using NodeAllocatorTraits = std::allocator_traits<NodeAllocator>;

    Node* head_;
    Node* tail_;
    std::size_t size_;
    NodeAllocator allocator_;

    void clear();
    Node* node_at(std::size_t index) const;

    template<typename U>
    Node* create_node(U&& value);

    void destroy_node(Node* node) noexcept;

public:
    class Iterator {
    private:
        Node* current_;

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
        const Node* current_;

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

    SinglyLinkedList();
    explicit SinglyLinkedList(const Alloc& allocator);
    SinglyLinkedList(const SinglyLinkedList& other);
    SinglyLinkedList(SinglyLinkedList&& other) noexcept;

    SinglyLinkedList& operator=(const SinglyLinkedList& other);
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept;

    ~SinglyLinkedList();

    void push_back(const T& value);
    void push_back(T&& value);

    void insert(std::size_t index, const T& value);
    void insert(std::size_t index, T&& value);
    void erase(std::size_t index);

    std::size_t size() const;
    bool empty() const;

    T& operator[](std::size_t index);
    const T& operator[](std::size_t index) const;

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;
};

template<typename T, typename Alloc>
SinglyLinkedList<T, Alloc>::SinglyLinkedList()
    : SinglyLinkedList(Alloc()) {
}

template<typename T, typename Alloc>
SinglyLinkedList<T, Alloc>::SinglyLinkedList(const Alloc& allocator)
    : head_(nullptr), tail_(nullptr), size_(0), allocator_(allocator) {
}

template<typename T, typename Alloc>
SinglyLinkedList<T, Alloc>::SinglyLinkedList(const SinglyLinkedList& other)
    : head_(nullptr),
      tail_(nullptr),
      size_(0),
      allocator_(NodeAllocatorTraits::select_on_container_copy_construction(other.allocator_)) {
    for (const T& value : other) {
        push_back(value);
    }
}

template<typename T, typename Alloc>
SinglyLinkedList<T, Alloc>::SinglyLinkedList(SinglyLinkedList&& other) noexcept
    : head_(other.head_),
      tail_(other.tail_),
      size_(other.size_),
      allocator_(std::move(other.allocator_)) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
}

template<typename T, typename Alloc>
SinglyLinkedList<T, Alloc>& SinglyLinkedList<T, Alloc>::operator=(const SinglyLinkedList& other) {
    if (this == &other) {
        return *this;
    }

    SinglyLinkedList copy(other);
    *this = std::move(copy);

    return *this;
}

template<typename T, typename Alloc>
SinglyLinkedList<T, Alloc>& SinglyLinkedList<T, Alloc>::operator=(SinglyLinkedList&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    clear();
    allocator_ = std::move(other.allocator_);
    head_ = other.head_;
    tail_ = other.tail_;
    size_ = other.size_;

    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;

    return *this;
}

template<typename T, typename Alloc>
SinglyLinkedList<T, Alloc>::~SinglyLinkedList() {
    clear();
}

template<typename T, typename Alloc>
void SinglyLinkedList<T, Alloc>::clear() {
    Node* current = head_;

    while (current != nullptr) {
        Node* next = current->next;
        destroy_node(current);
        current = next;
    }

    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
}

template<typename T, typename Alloc>
typename SinglyLinkedList<T, Alloc>::Node* SinglyLinkedList<T, Alloc>::node_at(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("SinglyLinkedList index is out of range");
    }

    Node* current = head_;

    for (std::size_t current_index = 0; current_index < index; ++current_index) {
        current = current->next;
    }

    return current;
}

template<typename T, typename Alloc>
template<typename U>
typename SinglyLinkedList<T, Alloc>::Node* SinglyLinkedList<T, Alloc>::create_node(U&& value) {
    Node* node = NodeAllocatorTraits::allocate(allocator_, 1);

    try {
        NodeAllocatorTraits::construct(allocator_, node, std::forward<U>(value));
    } catch (...) {
        NodeAllocatorTraits::deallocate(allocator_, node, 1);
        throw;
    }

    return node;
}

template<typename T, typename Alloc>
void SinglyLinkedList<T, Alloc>::destroy_node(Node* node) noexcept {
    NodeAllocatorTraits::destroy(allocator_, node);
    NodeAllocatorTraits::deallocate(allocator_, node, 1);
}

template<typename T, typename Alloc>
void SinglyLinkedList<T, Alloc>::push_back(const T& value) {
    Node* new_node = create_node(value);

    if (tail_ == nullptr) {
        head_ = new_node;
        tail_ = new_node;
    } else {
        tail_->next = new_node;
        tail_ = new_node;
    }

    ++size_;
}

template<typename T, typename Alloc>
void SinglyLinkedList<T, Alloc>::push_back(T&& value) {
    Node* new_node = create_node(std::move(value));

    if (tail_ == nullptr) {
        head_ = new_node;
        tail_ = new_node;
    } else {
        tail_->next = new_node;
        tail_ = new_node;
    }

    ++size_;
}

template<typename T, typename Alloc>
void SinglyLinkedList<T, Alloc>::insert(std::size_t index, const T& value) {
    if (index > size_) {
        throw std::out_of_range("SinglyLinkedList insert index is out of range");
    }

    if (index == size_) {
        push_back(value);
        return;
    }

    Node* new_node = create_node(value);

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

template<typename T, typename Alloc>
void SinglyLinkedList<T, Alloc>::insert(std::size_t index, T&& value) {
    if (index > size_) {
        throw std::out_of_range("SinglyLinkedList insert index is out of range");
    }

    if (index == size_) {
        push_back(std::move(value));
        return;
    }

    Node* new_node = create_node(std::move(value));

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

template<typename T, typename Alloc>
void SinglyLinkedList<T, Alloc>::erase(std::size_t index) {
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

    destroy_node(target);
    --size_;
}

template<typename T, typename Alloc>
std::size_t SinglyLinkedList<T, Alloc>::size() const {
    return size_;
}

template<typename T, typename Alloc>
bool SinglyLinkedList<T, Alloc>::empty() const {
    return size_ == 0;
}

template<typename T, typename Alloc>
T& SinglyLinkedList<T, Alloc>::operator[](std::size_t index) {
    return node_at(index)->data;
}

template<typename T, typename Alloc>
const T& SinglyLinkedList<T, Alloc>::operator[](std::size_t index) const {
    return node_at(index)->data;
}

template<typename T, typename Alloc>
typename SinglyLinkedList<T, Alloc>::Iterator SinglyLinkedList<T, Alloc>::begin() {
    return Iterator(head_);
}

template<typename T, typename Alloc>
typename SinglyLinkedList<T, Alloc>::Iterator SinglyLinkedList<T, Alloc>::end() {
    return Iterator(nullptr);
}

template<typename T, typename Alloc>
typename SinglyLinkedList<T, Alloc>::ConstIterator SinglyLinkedList<T, Alloc>::begin() const {
    return ConstIterator(head_);
}

template<typename T, typename Alloc>
typename SinglyLinkedList<T, Alloc>::ConstIterator SinglyLinkedList<T, Alloc>::end() const {
    return ConstIterator(nullptr);
}
