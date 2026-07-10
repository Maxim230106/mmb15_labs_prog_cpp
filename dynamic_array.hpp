#pragma once

#include <cstddef>   // std::size_t
#include <stdexcept> // std::out_of_range
#include <utility>   // std::move

template<typename T>
class DynamicArray {
private:
    T* data_;              // Указатель на начало выделенного буфера
    std::size_t size_;     // Количество реально сохраненных элементов
    std::size_t capacity_; // Размер выделенной памяти в элементах

    // Увеличивает буфер и переносит существующие элементы в новую память
    void reallocate(std::size_t new_capacity);

public:
    class Iterator {
    private:
        T* current_; // Текущая позиция итератора внутри буфера

    public:
        explicit Iterator(T* ptr)
            : current_(ptr) {
        }

        T& operator*() const {
            return *current_;
        }

        Iterator& operator++() {
            ++current_;
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
        const T* current_; // Текущая позиция константного итератора

    public:
        explicit ConstIterator(const T* ptr)
            : current_(ptr) {
        }

        const T& operator*() const {
            return *current_;
        }

        ConstIterator& operator++() {
            ++current_;
            return *this;
        }

        bool operator!=(const ConstIterator& other) const {
            return current_ != other.current_;
        }

        bool operator==(const ConstIterator& other) const {
            return current_ == other.current_;
        }
    };

    // Создает пустой массив без выделенной памяти
    DynamicArray();
    // Создает независимую копию другого массива
    DynamicArray(const DynamicArray& other);
    // Забирает ресурсы у временного объекта без копирования элементов
    DynamicArray(DynamicArray&& other) noexcept;

    // Полностью заменяет содержимое текущего массива копией other
    DynamicArray& operator=(const DynamicArray& other);
    // Передает владение буфером от временного объекта текущему массиву
    DynamicArray& operator=(DynamicArray&& other) noexcept;

    // Освобождает выделенную память
    ~DynamicArray();

    // Добавляет элемент по копии
    void push_back(const T& value);
    // Добавляет элемент с перемещением, если тип T это поддерживает
    void push_back(T&& value);

    // Вставляет элемент в указанную позицию, сдвигая хвост вправо
    void insert(std::size_t index, const T& value);
    // Вставляет элемент с перемещением в указанную позицию
    void insert(std::size_t index, T&& value);
    // Удаляет элемент и сдвигает оставшиеся элементы влево
    void erase(std::size_t index);

    // Возвращает текущее число элементов
    std::size_t size() const;
    // Проверяет, пуст ли массив
    bool empty() const;

    // Дает доступ к элементу по индексу с проверкой границ
    T& operator[](std::size_t index);
    // Константная версия доступа к элементу по индексу
    const T& operator[](std::size_t index) const;

    // Итератор на первый элемент
    Iterator begin();
    // Итератор на позицию сразу после последнего элемента
    Iterator end();

    // Константный итератор на первый элемент
    ConstIterator begin() const;
    // Константный итератор на позицию после последнего элемента
    ConstIterator end() const;
};

template<typename T>
DynamicArray<T>::DynamicArray()
    : data_(nullptr), size_(0), capacity_(0) {
}

template<typename T>
DynamicArray<T>::DynamicArray(const DynamicArray& other)
    : data_(nullptr), size_(other.size_), capacity_(other.capacity_) {
    // Если у исходного массива нет буфера, копировать нечего
    if (capacity_ == 0) {
        return;
    }

    data_ = new T[capacity_];

    for (std::size_t index = 0; index < size_; ++index) {
        data_[index] = other.data_[index];
    }
}

template<typename T>
DynamicArray<T>::DynamicArray(DynamicArray&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    // Переводим исходный объект в безопасное пустое состояние
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray& other) {
    if (this == &other) {
        return *this;
    }

    T* new_data = nullptr;

    if (other.capacity_ > 0) {
        new_data = new T[other.capacity_];

        // Копируем только занятые элементы, а не весь буфер
        for (std::size_t index = 0; index < other.size_; ++index) {
            new_data[index] = other.data_[index];
        }
    }

    delete[] data_;
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete[] data_;

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;

    return *this;
}

template<typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] data_;
}

template<typename T>
void DynamicArray<T>::reallocate(std::size_t new_capacity) {
    T* new_data = new T[new_capacity];

    // Перенос позволяет избежать лишнего копирования для тяжелых объектов
    for (std::size_t index = 0; index < size_; ++index) {
        new_data[index] = std::move(data_[index]);
    }

    delete[] data_;
    data_ = new_data;
    capacity_ = new_capacity;
}

template<typename T>
void DynamicArray<T>::push_back(const T& value) {
    if (size_ == capacity_) {
        // Растем плавно, чтобы не перевыделять память на каждом добавлении
        const std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ + capacity_ / 2 + 1;
        reallocate(new_capacity);
    }

    data_[size_] = value;
    ++size_;
}

template<typename T>
void DynamicArray<T>::push_back(T&& value) {
    if (size_ == capacity_) {
        // Используем ту же стратегию роста и для перемещаемой вставки
        const std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ + capacity_ / 2 + 1;
        reallocate(new_capacity);
    }

    data_[size_] = std::move(value);
    ++size_;
}

template<typename T>
void DynamicArray<T>::insert(std::size_t index, const T& value) {
    if (index > size_) {
        throw std::out_of_range("DynamicArray insert index is out of range");
    }

    if (size_ == capacity_) {
        const std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ + capacity_ / 2 + 1;
        reallocate(new_capacity);
    }

    // Сдвигаем элементы вправо, чтобы освободить позицию под вставку
    for (std::size_t current = size_; current > index; --current) {
        data_[current] = std::move(data_[current - 1]);
    }

    data_[index] = value;
    ++size_;
}

template<typename T>
void DynamicArray<T>::insert(std::size_t index, T&& value) {
    if (index > size_) {
        throw std::out_of_range("DynamicArray insert index is out of range");
    }

    if (size_ == capacity_) {
        const std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ + capacity_ / 2 + 1;
        reallocate(new_capacity);
    }

    // Сдвигаем элементы вправо, затем перемещаем новый элемент на свободное место
    for (std::size_t current = size_; current > index; --current) {
        data_[current] = std::move(data_[current - 1]);
    }

    data_[index] = std::move(value);
    ++size_;
}

template<typename T>
void DynamicArray<T>::erase(std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("DynamicArray erase index is out of range");
    }

    // После удаления подтягиваем хвост массива на одну позицию влево
    for (std::size_t current = index; current + 1 < size_; ++current) {
        data_[current] = std::move(data_[current + 1]);
    }

    --size_;
}

template<typename T>
std::size_t DynamicArray<T>::size() const {
    return size_;
}

template<typename T>
bool DynamicArray<T>::empty() const {
    return size_ == 0;
}

template<typename T>
T& DynamicArray<T>::operator[](std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("DynamicArray index is out of range");
    }

    return data_[index];
}

template<typename T>
const T& DynamicArray<T>::operator[](std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("DynamicArray index is out of range");
    }

    return data_[index];
}

template<typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::begin() {
    return Iterator(data_);
}

template<typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::end() {
    return Iterator(data_ + size_);
}

template<typename T>
typename DynamicArray<T>::ConstIterator DynamicArray<T>::begin() const {
    return ConstIterator(data_);
}

template<typename T>
typename DynamicArray<T>::ConstIterator DynamicArray<T>::end() const {
    return ConstIterator(data_ + size_);
}
