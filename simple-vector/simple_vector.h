#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include "array_ptr.h"
#include <iostream>
#include <utility>
#include <iterator>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve) :
        capacity_(capacity_to_reserve)
    {
    };
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value);

    // Перемещает вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, Type&& value);

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size);

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init);

    // Конструктор копирования
    SimpleVector(const SimpleVector& other);

    // Конструктор перемещения
    SimpleVector(SimpleVector&& other);

    // Конструктор вызова резервирования памяти
    SimpleVector(ReserveProxyObj obj);

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept;

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept;
    

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept;
    

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept;

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept;

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index);

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const;

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept;

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size);

    void Reserve(size_t new_capacity);
    
    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept;

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept;

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept;

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept;

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept;

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept;

    // Оператор присваивания
    SimpleVector& operator=(const SimpleVector& rhs);

    // Оператор присваивания через move
    SimpleVector& operator=(SimpleVector&& rhs);

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item);

    // Добавляет элемент в конец вектора через move
    void PushBack(Type&& item);

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value);

    //Вставка перемещением
    Iterator Insert(ConstIterator pos, Type&& value);

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept;

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos);

    //Удаление с move
    Iterator Erase(Iterator pos);

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept;

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
SimpleVector<Type>::SimpleVector(size_t size, const Type& value) :
    items_(size),
    size_(size),
    capacity_(size)
{
    std::fill(items_.Get(), items_.Get() + size, value);
}

template <typename Type>
SimpleVector<Type>::SimpleVector(size_t size, Type&& value) :
    items_(size),
    size_(size),
    capacity_(size)
{
    ArrayPtr<Type> tmp(size_);
    for (size_t i = 0; i < size; ++i) {
        tmp[i] = std::move(value);
    }
    items_.swap(tmp);
}

template <typename Type>
SimpleVector<Type>::SimpleVector(size_t size) : SimpleVector<Type>::SimpleVector(size, Type())
{
}

template <typename Type>
SimpleVector<Type>::SimpleVector(std::initializer_list<Type> init) :
    items_(init.size()),
    size_(init.size()),
    capacity_(init.size())
{
    std::copy(init.begin(), init.end(), items_.Get());
}

template <typename Type>
SimpleVector<Type>::SimpleVector(const SimpleVector& other) :
    items_(other.GetSize()),
    size_(other.GetSize()),
    capacity_(other.GetCapacity())
{
    SimpleVector<Type> tmp(size_);
    std::copy(other.begin(), other.end(), tmp.begin());
    items_.swap(tmp.items_);
}

template <typename Type>
SimpleVector<Type>::SimpleVector(SimpleVector&& other) :
    items_(other.GetSize()),
    size_(other.GetSize()),
    capacity_(other.GetCapacity())
{
    std::move(other.begin(), other.end(), begin());
    std::exchange(other.size_, 0);
    std::exchange(other.capacity_, 0);
}

template <typename Type>
SimpleVector<Type>::SimpleVector(ReserveProxyObj obj) {
    Reserve(obj.capacity_);
}

template <typename Type>
size_t SimpleVector<Type>::GetSize() const noexcept {
    return size_;
}

template <typename Type>
size_t SimpleVector<Type>::GetCapacity() const noexcept {
    return capacity_;
}

template <typename Type>
bool SimpleVector<Type>::IsEmpty() const noexcept {
    return size_ == 0;
}

template <typename Type>
Type* SimpleVector<Type>::begin() noexcept {
    return items_.Get();
}

template <typename Type>
Type* SimpleVector<Type>::end() noexcept {
    return (items_.Get() + size_);
}

template <typename Type>
const Type* SimpleVector<Type>::begin() const noexcept {
    return items_.Get();
}

template <typename Type>
const Type* SimpleVector<Type>::end() const noexcept {
    return (items_.Get() + size_);
}

template <typename Type>
const Type* SimpleVector<Type>::cbegin() const noexcept {
    return items_.Get();
}

template <typename Type>
const Type* SimpleVector<Type>::cend() const noexcept {
    return (items_.Get() + size_);
}

template <typename Type>
Type& SimpleVector<Type>::operator[](size_t index) noexcept {
    assert(index < size_);
    return items_[index];
}

template <typename Type>
const Type& SimpleVector<Type>::operator[](size_t index) const noexcept {
    assert(index < size_);
    return const_cast<Type&>(items_[index]);
}

template <typename Type>
Type& SimpleVector<Type>::At(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Index >= size!");
    }
    else
    {
        return items_[index];
    }
}

template <typename Type>
const Type& SimpleVector<Type>::At(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Index >= size!");
    }
    else
    {
        return items_[index];
    }
}

template <typename Type>
void SimpleVector<Type>::Clear() noexcept {
    size_ = 0;
}

template <typename Type>
void SimpleVector<Type>:: Resize(size_t new_size) {
    if (new_size >= size_)
    {
        if (capacity_ >= new_size)
        {
            for (size_t i = size_; i < new_size; ++i) {
                items_[i] = std::move(Type());
            }
        }
        else
        {
            SimpleVector<Type> tmp(std::max(this->capacity_ * 2, new_size));
            std::move(begin(), end(), tmp.begin());
            tmp.size_ = new_size;
            for (size_t i = size_; i < new_size; ++i) {
                tmp.items_[i] = std::move(Type());
            }
            *this = std::move(tmp);
            this->capacity_ = std::max(this->capacity_ * 2, new_size);
        }
    }
    size_ = new_size;
}

template <typename Type>
void SimpleVector<Type>::Reserve(size_t new_capacity) {
    if (new_capacity > capacity_) {
        ArrayPtr<Type> new_cont_(new_capacity);
        std::move(items_.Get(), items_.Get() + size_, new_cont_.Get());
        items_.swap(new_cont_);
        capacity_ = new_capacity;
    }
}

template <typename Type>
SimpleVector<Type>& SimpleVector<Type>::operator=(const SimpleVector& rhs) {
    if (*this == rhs) {
        return *this;
    }
    SimpleVector<Type> tmp(rhs);
    this->swap(tmp);
    return *this;
}

template <typename Type>
SimpleVector<Type>& SimpleVector<Type>::operator=(SimpleVector&& rhs) {
    items_ = std::move(rhs.items_);
    size_ = std::move(rhs.size_);
    rhs.capacity_ = 0;
    rhs.size_ = 0;
    return *this;
}

template <typename Type>
void SimpleVector<Type>::PushBack(const Type& item) {
    this->Resize(size_ + 1);
    items_[size_ - 1] = item;
}

template <typename Type>
void SimpleVector<Type>::PushBack(Type&& item) {
    this->Resize(size_ + 1);
    items_[size_ - 1] = std::move(item);
}

template <typename Type>
Type* SimpleVector<Type>::Insert(ConstIterator pos, const Type& value) {
    assert(pos >= begin() && pos <= end());
    auto diffb = std::distance(cbegin(), pos);
    Iterator pos_ = begin() + diffb;
    if (capacity_ <= size_)
    {
        if (capacity_ == 0)
        {
            this->PushBack(value);
            return this->begin();
        }
        ArrayPtr<Type> tmp(capacity_ * 2);
        std::copy(begin(), pos_, tmp.Get());
        tmp[diffb] = value;
        std::copy_backward(pos_, end(), tmp.Get() + size_ + 1);
        ++size_;
        capacity_ *= 2;
        items_.swap(tmp);
        return &items_[diffb];
    }
    std::copy_backward(pos_, end(), items_.Get() + std::next(this->end()));
    items_[diffb] = value;
    ++size_;
    return &items_[diffb];
}

template <typename Type>
Type* SimpleVector<Type>::Insert(ConstIterator pos, Type&& value) {
    assert(pos >= begin() && pos <= end());
    auto diffb = std::distance(cbegin(), pos);
    Iterator pos_ = begin() + diffb;
    if (capacity_ <= size_)
    {
        if (capacity_ == 0)
        {
            this->PushBack(std::move(value));
            return this->begin();
        }
        ArrayPtr<Type> tmp(capacity_ * 2);
        std::move(begin(), pos_, tmp.Get());
        tmp[diffb] = std::move(value);
        std::move_backward(pos_, end(), tmp.Get() + size_ + 1);
        ++size_;
        capacity_ *= 2;
        items_ = std::move(tmp);
        return &items_[diffb];
    }
    std::move_backward(pos_, end(), std::next(this->end()));
    items_[diffb] = std::move(value);
    ++size_;
    return &items_[diffb];
}

template <typename Type>
void SimpleVector<Type>::PopBack() noexcept {
    assert(!IsEmpty());
        --size_;    
}

template <typename Type>
Type* SimpleVector<Type>::Erase(ConstIterator pos) {
    assert(pos >= begin() && pos < end());
    assert(!IsEmpty());
    auto distance = pos - cbegin();
    Iterator pos_ = items_.Get() + distance;
    std::copy(pos_ + 1, end(), pos_);
    --size_;
    return pos_;
}

template <typename Type>
Type* SimpleVector<Type>::Erase(Iterator pos) {
    assert(pos >= begin() && pos < end());
    assert(!IsEmpty());
    std::move(std::next(pos), end(), const_cast<Iterator>(pos));
    --size_;
    return const_cast<Iterator>(pos);
}

template <typename Type>
void SimpleVector<Type>::swap(SimpleVector& other) noexcept {
    items_.swap(other.items_);
    std::swap(other.size_, size_);
    std::swap(other.capacity_, capacity_);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (&lhs == &rhs) || std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}