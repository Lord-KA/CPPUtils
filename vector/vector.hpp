#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <memory>
#include <utility>
#include <cassert>
#include <type_traits>
#include <algorithm>

namespace g {

template<typename T, class Allocator = std::allocator<T>>
class vector {
private:

    using value_type      = T;
    using allocator_type  = Allocator;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    Allocator allocator_;
    T *data_;
    size_type capacity_;
    size_type size_;


public:
    //====================================
    //  Member functions

    constexpr vector() noexcept(noexcept(Allocator())); // TODO test perfomance with and without noexcept

    constexpr explicit vector(const Allocator& alloc) noexcept;

    constexpr explicit vector(size_type count,
                               const T& value,
                               const Allocator& alloc = Allocator());

    constexpr explicit vector(size_type count,
                               const Allocator& alloc = Allocator());

    template<class InputIt>
    constexpr explicit vector(InputIt first, InputIt last,
                               const Allocator& alloc = Allocator());

    constexpr vector(const vector& other);

    constexpr vector(const vector& other, const Allocator& alloc);

    constexpr vector(vector&& other);

    constexpr vector(vector&& other, const Allocator& alloc);

    constexpr vector(std::initializer_list<T> init,
                      const Allocator& alloc = Allocator());

    constexpr ~vector();


    constexpr vector& operator=(const vector& other);

    constexpr vector& operator=(vector&& other) noexcept;

    constexpr vector& operator=(std::initializer_list<T> ilist);


    constexpr void assign(size_type count, const T& value);

    template<class InputIt>
    constexpr void assign(InputIt first, InputIt last);

    constexpr void assign(std::initializer_list<T> ilist);


    constexpr allocator_type get_allocator() const noexcept { return allocator_; }

    //====================================
    //  Element access

    constexpr T& at(size_type pos);

    constexpr const T& at(size_type pos) const;


    constexpr T& operator[](size_type pos);

    constexpr const T& operator[](size_type pos) const;


    constexpr T& front()             { assert(size_ != 0); return data_[0]; }

    constexpr const T& front() const { assert(size_ != 0); return data_[0]; }

    constexpr T& back()              { assert(size_ != 0); return data_[size_ - 1]; }

    constexpr const T& back()  const { assert(size_ != 0); return data_[size_ - 1]; }


    //====================================
    //  Iterators

    struct iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

    public:
        explicit iterator(const size_t id, const vector* this_) : id_(id), this_(this_) {}
        iterator(const iterator& other) = default;

        bool operator==(const iterator &other) const { return id_ == other.id_; }
        bool operator!=(const iterator &other) const { return id_ != other.id_; }

        bool operator< (const iterator &other) const { return id_ <  other.id_; }
        bool operator> (const iterator &other) const { return id_ >  other.id_; }
        bool operator<=(const iterator &other) const { return id_ <= other.id_; }
        bool operator>=(const iterator &other) const { return id_ >= other.id_; }

        T& operator*()
        {
            assert(id_ < this_->size());
            return this_->data_[id_];
        }

        const T& operator*() const
        {
            assert(id_ <this_->size());
            return this_->data_[id_];
        }

        T& operator[](long long int n)
        {
            assert(id_ + n <this_->size());
            return this_->data_[id_ + n];
        }

        const T& operator[](long long int n) const
        {
            assert(id_ + n <this_->size());
            return this_->data_[id_ + n];
        }

        size_t getId() const
        {
            return id_;
        }

        iterator operator++()
        {
            ++id_;
            return *this;
        }

        iterator operator++(int)
        {
            iterator result(*this);
            ++id_;
            return result;
        }

        iterator operator--()
        {
            --id_;
            return *this;
        }

        iterator operator--(int)
        {
            iterator result(*this);
            --id_;
            return result;
        }

        iterator operator+(long long int n) const
        {
            iterator result(*this);
            result.id_ += n;
            return result;
        }

        iterator operator+(const iterator& other) const
        {
            iterator result(other);
            result.id_ += id_;
            return result;
        }

        long long operator-(const iterator& other) const
        {
            return id_ - other.id_;
        }

        iterator operator-(long long int n) const
        {
            iterator result(*this);
            result.id_ -= n;
            return result;
        }

        iterator operator+=(long long int n)
        {
            id_ += n;
            return (*this);
        }

        iterator operator-=(long long int n)
        {
            id_ -= n;
            return (*this);
        }


    private:
        size_t id_;
        const vector* this_;
    };

    constexpr iterator begin() const noexcept
    {
        return iterator(0, this);
    }

    constexpr iterator end() const noexcept
    {
        return iterator(this->size(), this);
    }


    //====================================
    //  Capacity

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return begin() == end();
    }

    constexpr size_type size() const noexcept
    {
        return size_;
    }

    constexpr size_type max_size() const noexcept
    {
        return allocator_.max_size();   //TODO check if it is the right way to define max_size
    }

    constexpr void reserve(size_type new_cap);

    constexpr size_type capacity() const noexcept
    {
        return capacity_;
    }

    constexpr void shrink_to_fit();


    //====================================
    //  Modifiers

    constexpr void clear() noexcept;

    constexpr iterator insert(iterator pos, const T& value);

    constexpr iterator insert(iterator pos, T&& value);

    constexpr iterator insert(iterator pos, const size_type count, const T& value);

    template<class InputIt>
    constexpr iterator insert(iterator pos, InputIt first, InputIt last);

    constexpr iterator insert(iterator pos, std::initializer_list<T> ilist);

    template<class... Args>
    constexpr iterator emplace(const iterator pos, Args&&... args);

    constexpr iterator erase(const iterator pos);

    constexpr iterator erase(const iterator first, const iterator last);

    constexpr void push_back(const T& value);

    constexpr void push_back(T&& value);

    template<class... Args>
    constexpr T* emplace_back(Args&&... args);

    constexpr void pop_back() noexcept { assert(size_ != 0); resize(size_ - 1); };

    constexpr void resize(size_type count, const T& value = T());

    constexpr void swap(vector& other) noexcept;


    //====================================
    //  Comparing

    template<typename Container>
    constexpr bool operator==(const Container& other) const;

    template<typename Container>
    constexpr bool operator!=(const Container& other) const;

private:

    template<class InputIt>
    constexpr void _vector_iters_constructor(InputIt first, InputIt last, const std::false_type& /* IsIntegral */)
    {
        if (data_)
            allocator_.deallocate(data_, capacity_);
        capacity_ = 0;
        size_ = 0;
        data_ = nullptr;
        size_t distance = std::distance(first, last);
        resize(distance);
        std::copy(first, last, data_);
    }

    template<class Integer>
    constexpr void _vector_iters_constructor(Integer n, Integer val, const std::true_type& /* IsIntegral */)
    {
        if (data_)
            allocator_.deallocate(data_, capacity_);
        capacity_ = n;
        size_ = n;
        data_ = allocator_.allocate(capacity_);
        if (!data_)
            throw std::bad_alloc();
        std::fill(data_, data_ + capacity_, val);
    }


    template<class InputIt>
    constexpr iterator _vector_iters_insert(const iterator pos, InputIt first, InputIt last, const std::false_type& /* IsIntegral */)
    {
        size_t distance = std::distance(first, last);
        reserve(size_ + distance);
        size_ += distance;
        std::copy(data_ + pos.getId(), data_ + size_ - distance, data_ + pos.getId() + distance);
        std::copy(first, last, begin() + pos);
        return pos;
    }

    template<class Integer>
    constexpr iterator _vector_iters_insert(const iterator pos, Integer count, Integer value, const std::true_type& /* IsIntegral */)
    {
        std::cerr << "HERE!\n";
        reserve(size_ + count);
        size_ += count;
        std::move_backward(pos, iterator(size_ - count, this), iterator(size_ - count, this));
        std::fill(begin() + pos, begin() + pos + count, value);
        return pos;
    }


#ifndef NDEBUG

public:

    void dump(std::ostream& out)
    {
        out << "capacity = " << capacity_ << "\n";
        out << "size     = " << size_ << "\n";
        for (auto elem : *this)
            out << "{ " << elem << " } ";
        out << '\n';
    }

#endif
};



template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector() noexcept(noexcept(Allocator()))
        : data_(nullptr), capacity_(0), size_(0) {}


template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(const Allocator& alloc) noexcept
        : allocator_(alloc), data_(nullptr), capacity_(0), size_(0) {}


template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(size_type count, const T& value, const Allocator& alloc)
        : allocator_(alloc), data_(nullptr), capacity_(count), size_(count)
{
    data_ = allocator_.allocate(capacity_);
    if (!data_)
        throw std::bad_alloc();
    std::fill(data_, data_ + capacity_, value);
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(size_type count, const Allocator& alloc)
        : allocator_(alloc), data_(nullptr), capacity_(count), size_(count)
{
    data_ = allocator_.allocate(capacity_);
    if (!data_)
        throw std::bad_alloc();
    for (size_t i = 0; i < count; ++i)
        std::construct_at(data_ + i);
}


template<typename T, class Allocator>
template<class InputIt>
constexpr vector<T, Allocator>::vector(InputIt first, InputIt last, const Allocator& alloc)
       : allocator_(alloc), data_(nullptr)
{
    _vector_iters_constructor(first, last, typename std::is_integral<InputIt>::type());
}



template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(const vector& other)
        : allocator_(other.allocator_), data_(nullptr), capacity_(other.capacity_), size_(other.size_)
{
    data_ = allocator_.allocate(capacity_);
    if (!data_)
        throw std::bad_alloc();
    std::copy(other.begin(), other.end(), data_);
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(const vector& other, const Allocator& alloc)
        : allocator_(alloc), capacity_(other.capacity_), size_(other.size_)
{
    data_ = allocator_.allocate(capacity_);
    if (!data_)
        throw std::bad_alloc();
    std::copy(other.begin(), other.end(), data_);
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(vector&& other)
        : allocator_(other.allocator_), data_(other.data_), capacity_(other.capacity_), size_(other.size_)
{
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(std::initializer_list<T> ilist, const Allocator& alloc)
        : allocator_(alloc), data_(nullptr)
{
    _vector_iters_constructor(ilist.begin(), ilist.end(), std::false_type());
}

template<typename T, class Allocator>
constexpr vector<T, Allocator>::vector(vector&& other, const Allocator& alloc)
        : allocator_(alloc), data_(nullptr), capacity_(other.capacity_), size_(other.size_)
{
    if (alloc != other.allocator_){
        data_ = allocator_.allocate(capacity_);
        if (!data_)
            throw std::bad_alloc();

        if (std::is_move_assignable<T>::value)
            for (size_t i = 0; i < size_; ++i)
                data_[i] = std::move(other.data_[i]);
        else
            std::copy(other.begin(), other.end(), data_);
    }
    else {
        data_ = other.data_;
        other.size_ = 0;
        other.capacity_ = 0;
    }
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>::~vector()
{
    if (data_)
        allocator_.deallocate(data_, capacity_);
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>& vector<T, Allocator>::operator=(const vector& other)
{
    size_ = other.size_;
    if (data_)
        allocator_.deallocate(data_, capacity_);
    capacity_ = other.capacity_;
    data_ = allocator_.allocate(capacity_);
    if (!data_)
        throw std::bad_alloc();
    std::copy(other.begin(), other.end(), data_);
    return *this;
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>& vector<T, Allocator>::operator=(vector&& other) noexcept
{
    if (data_)
        allocator_.deallocate(data_, capacity_);    // TODO handle exceptions

    allocator_ = other.allocator_;
    capacity_  = std::exchange(other.capacity_, 0);
    size_ = std::exchange(other.size_, 0);
    data_ = std::exchange(other.data_, nullptr);
    return *this;
}


template<typename T, class Allocator>
constexpr vector<T, Allocator>& vector<T, Allocator>::operator=(std::initializer_list<T> ilist)
{
    _vector_iters_constructor(ilist.begin(), ilist.end(), std::false_type());
    return *this;
}


template<typename T, class Allocator>
constexpr void vector<T, Allocator>::assign(size_type count, const T& value)
{
    if (capacity_ <count){
        allocator_.deallocate(data_, capacity_);
        capacity_ = count;
        data_ = allocator_.allocate(capacity_);
        if (!data_)
            throw std::bad_alloc();
    }
    size_ = count;
    std::fill(data_, data_ + size_, value);
}


template<typename T, class Allocator>
template<class InputIt>
constexpr void vector<T, Allocator>::assign(InputIt first, InputIt last)
{
    _vector_iters_constructor(first, last, typename std::is_integral<InputIt>::type());
}

template<typename T, class Allocator>
constexpr void vector<T, Allocator>::assign(std::initializer_list<T> ilist)
{
    _vector_iters_constructor(ilist.begin(), ilist.end(), std::false_type());
}



template<typename T, class Allocator>
constexpr T& vector<T, Allocator>::at(size_type pos)
{
    if (pos>= size())
        throw std::out_of_range("Position given to at() is invalid");
    return data_[pos];
}


template<typename T, class Allocator>
constexpr const T& vector<T, Allocator>::at(size_type pos) const
{
    if (pos>= size())
        throw std::out_of_range("Position given to at() is invalid");
    return data_[pos];
}


template<typename T, class Allocator>
constexpr T& vector<T, Allocator>::operator[](size_type pos)
{
    assert(pos < size());
    return data_[pos];
}


template<typename T, class Allocator>
constexpr const T& vector<T, Allocator>::operator[](size_type pos) const {
    assert(pos < size());
    return data_[pos];
}


template<typename T, class Allocator>
constexpr void vector<T, Allocator>::reserve(size_type new_cap)
{
    if (new_cap > capacity_){
        T *tmp = allocator_.allocate(new_cap);
        if (!tmp)
            throw std::bad_alloc();

        if (data_){
            std::copy(data_, data_ + size_, tmp);
            allocator_.deallocate(data_, capacity_);
        }
        data_ = tmp;
        capacity_ = new_cap;
        return;
    }
}


template<typename T, class Allocator>
constexpr void vector<T, Allocator>::shrink_to_fit()
{
    if (size_ != capacity_){
        T *tmp = allocator_.allocate(size_);
        if (!tmp)
            throw std::bad_alloc();

        std::copy(data_, data_ + size_, tmp);
        assert(data_);
        allocator_.deallocate(data_, capacity_);
        data_ = tmp;
        capacity_ = size_;
    }
}


template<typename T, class Allocator>
constexpr void vector<T, Allocator>::clear() noexcept
{
    if (data_)
        allocator_.deallocate(data_, capacity_);    // TODO handle exceptions
    size_ = 0;
    capacity_ = 0;
    data_ = nullptr;
}


template<typename T, class Allocator>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(vector<T, Allocator>::iterator pos, const T& value)
{
    assert(false);
    reserve(size_ + 1);
    // std::move_backward(std::move(pos), std::move(pos + 1), std::move(iterator(size_, this)));
    ++size_;
    std::shift_right(pos, iterator(size_ - 1), 1);
    // std::copy(data_ + pos.getId(), data_ + size_ - 1, data_ + pos.getId() + 1);
    // *pos = value;
    return pos;
}

template<typename T, class Allocator>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(vector<T, Allocator>::iterator pos, T&& value)
{
    reserve(size_ + 1);
    ++size_;
    const size_type shift = 1;
    // std::shift_right(pos, iterator(size_, this), shift);
    std::move_backward(pos, iterator(size_ - 1, this), iterator(size_, this));
    // std::copy(data_ + pos.getId(), data_ + size_ - 1, data_ + pos.getId() + 1);
    *pos = std::move(value);
    return pos;
}

template<typename T, class Allocator>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const vector<T, Allocator>::iterator pos, const size_type count, const T& value)
{
    reserve(size_ + count);
    ++size_;
    std::shift_right(pos, iterator(size_, this), count);
    // std::copy(data_ + pos.getId(), data_ + size_ - count, data_ + pos.getId() + count);
    std::fill(begin() + pos, begin() + pos + count, value);
    return pos;
}

template<typename T, class Allocator>
template<class InputIt>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const typename vector<T, Allocator>::iterator pos, InputIt first, InputIt last)
{
    return _vector_iters_insert(pos, first, last, typename std::is_integral<InputIt>::type());
}

template<typename T, class Allocator>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const iterator pos, std::initializer_list<T> ilist)
{
    return _vector_iters_insert(pos, ilist.begin(), ilist.end(), std::false_type());
}


template<typename T, class Allocator>
template<class... Args>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::emplace(const iterator pos, Args&&... args)
{
    return insert(pos, std::forward<T>(T(args...)));
}

template<typename T, class Allocator>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(const iterator pos)
{
    std::copy(data_ + pos.getId() + 1, data_ + size_, data_ + pos.getId());
    resize(size_ - 1);
    return pos;
}

template<typename T, class Allocator>
constexpr typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(const iterator beg, const iterator end)
{
    std::copy(data_ + end.getId(), data_ + size_, data_ + beg.getId());
    resize(size_ - (end - beg));
    return beg;
}

template<typename T, class Allocator>
constexpr void vector<T, Allocator>::push_back(const T& value)
{
    if ((size_ == capacity_) || !capacity_){
        if (!capacity_)
            capacity_ = 1;
        reserve(capacity_ * 2);
    }

    data_[size_] = value;
    ++size_;
}


template<typename T, class Allocator>
constexpr void vector<T, Allocator>::push_back(T&& value)
{
    if (size_ == capacity_ || !capacity_){
        if (!capacity_)
            capacity_ = 1;
        reserve(capacity_ * 2);
    }

    data_[size_] = std::move(value);
    ++size_;
}


template<typename T, class Allocator>
template<class... Args>
constexpr T* vector<T, Allocator>::emplace_back(Args&&... args)
{
    reserve(size_ + 1);
    std::construct_at(data_ + size_, args...);
    ++size_;
    return data_ + size_ - 1;
}


template<typename T, class Allocator>
constexpr void vector<T, Allocator>::resize(size_type count, const T& value)
{
    if (count == size_)
        return;

    if (count < size_) {
        do {
            --size_;
            std::destroy_at(data_ + size_); //TODO handle exceptions
        } while (size_ > count);
    } else if (count > size_){
        T *new_data = allocator_.allocate(count);
        if (!new_data)
            throw std::bad_alloc();

        std::copy(data_, data_ + size_, new_data);
        std::fill(new_data + size_, new_data + count, value);
        if (data_)
            allocator_.deallocate(data_, capacity_);

        data_ = new_data;
        size_ = count;
        capacity_ = count;
    }
}

template<typename T, class Allocator>
constexpr void vector<T, Allocator>::swap(vector& other) noexcept
{
    std::swap(allocator_, other.allocator_);
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
}

template<typename T, class Allocator>
template<typename Container>
constexpr bool vector<T, Allocator>::operator==(const Container& other) const {
    if (size() != other.size())
        return false;
    auto iter_this = begin();
    auto iter_other = other.begin();
    while (iter_this != end()){
        if (*iter_this != *iter_other)
            return false;
        ++iter_this; ++iter_other;
    }

    return true;

}

template<typename T, class Allocator>
template<typename Container>
constexpr bool vector<T, Allocator>::operator!=(const Container& other) const
{
    return !(*this == other);
}

}
#endif
