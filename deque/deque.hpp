#ifndef DEQUE_HPP
#define DEQUE_HPP

#include <iostream>
#include <cassert>
#include <algorithm>
#include <utility>

namespace g {
#ifndef NDEBUG                              //WARNING: debug features will fail with types smaller than int

#define DEQUE_CHECK(v) { \
    health_error error = HealthCheck(); \
    if (error != health_error::none) \
    { \
        std::cerr << "Deque "; \
        if (error == health_error::size) \
            std::cerr << "size"; \
        if (error == health_error::empty) \
            std::cerr << "empty"; \
        if (error == health_error::overflow) \
            std::cerr << "overflow"; \
        if (error == health_error::nooverflow) \
            std::cerr << "nooverflow"; \
        std::cerr << " error in " << __func__ << " at " << __LINE__ << "\n"; \
        std::cerr.flush(); \
        dump(std::cerr); \
        std::exit(0); \
    } }

#else

#define DEQUE_CHECK(v)

#endif


template<typename T>
class deque{
private:
    #ifndef NDEBUG
        using poison_t = int;
        static constexpr int POISON = 0xDEADC0DE;

        template<typename K>
        static void fillPoison(K* data)
        {
            static_assert(sizeof(K) >= sizeof(int));
            std::fill(reinterpret_cast<poison_t*>(data), reinterpret_cast<poison_t*>(data) + sizeof(*data) / sizeof(poison_t), POISON);
        }

        template<typename K>
        static bool isPoisoned(K* data)
        {
            return (reinterpret_cast<poison_t*>(data)[0] == POISON);
        }

enum class health_error { none, size, empty, nooverflow, overflow };


    #endif

    T* data;
    size_t capacity_;           // Capacity is a number power of two - 1; thus pos & capacity_ == pos % capacity_ <=> it takes into account overflow of a tip of deque
    size_t begin_;              // id of a first element (can be smaller than end_)
    size_t end_;                // id of the last element
    size_t size_;               // independent counter of size of deque

public:

    //===========================================
    // Interface functions

    deque( size_t size = 0 );
    deque( const deque &other );
    deque( deque &&other );

    ~deque() { delete[] data; }

    void push_back( const T& val );
    T    pop_back();
    void push_front( const T& val );
    T    pop_front();
    deque& operator=( const deque &other );
    deque& operator=( deque &&other );

    bool operator==( const deque &other ) const;
    bool operator!=( const deque &other ) const { return !(*this == other); }

    template<class U>
    bool operator==( const U &other ) const;
    template<class U>
    bool operator!=( const U &other ) const { return !(*this == other); }

    void refit( size_t capacity_ = -1 );                   //refit() fits data to the len OR reallocates memory

    void insert( size_t pos, const T& value );
    T erase( size_t pos );


          T& operator[]( size_t pos )       { return data[(pos + begin_) & capacity_]; }
    const T& operator[]( size_t pos ) const { return data[(pos + begin_) & capacity_]; }

    size_t size() const { return size_; }


    #ifndef NDEBUG
    //===========================================
    // Debug funcs


    void dump(std::ostream &out) {
        bool flag = true;
        out << "capacity = " << this->capacity_ << '\n';
        out << "size  = " << this->size() << '\n';
        out << "begin = " << this->begin_ << '\n';
        out << "end   = " << this->end_   << '\n';
        if (!data){
            out << "EMPTY!\n";
            return;
        }
        for (size_t i = 0; i <= capacity_; ++i){
            if (!flag)
                out << ", ";
            out << "{ ";
            if (i == begin_)
                out << "|beg| ";
            if (i == end_)
                out << "|end| ";
            out << data[i] << " }";
            flag = false;
        }
        out << '\n';
    }

    health_error HealthCheck() {
        if (size_ > capacity_ + 1)
            return health_error::size;

        if (!size_){
            for (size_t i = 0; i <= capacity_; ++i)
                if (!isPoisoned(&data[i]))
                    return health_error::empty;
            return health_error::none;
        }

        if (begin_ <= end_)
            for (size_t i = 0; i <= capacity_; ++i){
                if (begin_ <= i && i <= end_){
                    if (isPoisoned(&data[i]))
                        return health_error::nooverflow;
                }
                else
                    if (!isPoisoned(&data[i]))
                        return health_error::nooverflow;
            }
        else
            for (size_t i = 0; i <= capacity_; ++i){
                if (end_ < i && i < begin_){
                    if (!isPoisoned(&data[i]))
                        return health_error::overflow;
                }
                else
                    if (isPoisoned(&data[i]))
                        return health_error::overflow;
            }
        return health_error::none;
    }
    #endif




    //===========================================
    // Iterators

    struct Iterator{

    private:
        size_t id;                   // id in data_
        size_t pos;                  // pos in deque
        const deque *this_;


    public:

        using iterator_category = std::random_access_iterator_tag;

        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        Iterator( size_t id, size_t pos, const deque<T>* this_ = nullptr ) : id(id), pos(pos), this_(this_) {}
        Iterator( const Iterator& other) : id(other.id), pos(other.pos), this_(other.this_) {}

        bool operator==( const Iterator& other ) const { return pos == other.pos; }
        bool operator!=( const Iterator& other ) const { return pos != other.pos;  }

        bool operator< ( const Iterator& other ) const { return pos <  other.pos; }
        bool operator> ( const Iterator& other ) const { return pos >  other.pos; }
        bool operator<=( const Iterator& other ) const { return pos <= other.pos; }
        bool operator>=( const Iterator& other ) const { return pos >= other.pos; }

        T& operator*()             { assert(id <= this_->capacity_); return this_->data[id]; }

        const T& operator*() const { assert(id <= this_->capacity_); return this_->data[id]; }


        Iterator operator++(){
            ++pos;
            ++id;
            id &= this_->capacity_;

            return (*this);
        }

        Iterator operator++(int){
            Iterator result(*this);
            ++pos;
            ++id;
            id &= this_->capacity_;

            return result;
        }

        Iterator operator--(){
            --pos;
            --id;
            id &= this_->capacity_;
            return (*this);
        }

        Iterator operator--(int){
            Iterator result(*this);
            --pos;
            --id;
            id &= this_->capacity_;
            return result;
        }

        Iterator operator+(long long int n) const {
            Iterator result(*this);
            result.pos += n;
            result.id += n;
            result.id &= this_->capacity_;
            return result;
        }

        long long operator-(const Iterator& other) const {
            return pos - other.pos;
        }

        Iterator operator-(long long int n) const {
            Iterator result(*this);
            result.pos -= n;
            result.id -= n;
            result.id &= this_->capacity_;
            return result;
        }

        Iterator operator+=(long long int n){
            pos += n;
            id += n;
            id &= this_->capacity_;
            return (*this);
        }

        Iterator operator-=(long long int n){
            pos -= n;
            id -= n;
            id &= this_->capacity_;
            return (*this);
        }


    };

    Iterator begin() { return Iterator( begin_, 0, this); }
    Iterator end()   { return Iterator( end_ + 1, size(), this); }

    Iterator begin() const { return Iterator( begin_, 0, this); }
    Iterator end()   const { return Iterator( end_ + 1, size(), this); }

};




template<typename T>
deque<T>::deque( const deque &other )
    : data(nullptr), capacity_(other.capacity_), begin_(other.begin_), end_(other.end_), size_(other.size_) {

    if (capacity_){
        data = new T[capacity_ + 1];
        std::copy(other.data, other.data + capacity_ + 1, data);
    }

    DEQUE_CHECK(*this)
}


template<typename T>
deque<T>::deque( deque &&other )
    : capacity_(other.capacity_), data(other.data), begin_(other.begin_), end_(other.end_), size_(other.size_) {

    other.data = nullptr;
    other.capacity_ = 0;
    other.begin_    = 0;
    other.end_      = 0;
    other.size_     = 0;

    DEQUE_CHECK(*this)
}


template<typename T>
deque<T>::deque( size_t size)
    : data(nullptr), capacity_(0), begin_(0), end_(0), size_(0) {

    if (!size)
        return;
    size_t i = 2;
    while (i < size)
        i <<= 1;
    capacity_ = i - 1;
    data = new T[capacity_ + 1];

    #ifndef NDEBUG
    for (size_t j = 0; j <= capacity_; ++j)
        fillPoison(&data[j]);
    #endif

    DEQUE_CHECK(*this)
}



template<typename T>
void deque<T>::push_back(const T &val) {
    refit();
    if (!size_){
        data[0] = val;
    }
    else {
        end_ = (end_ + 1) & capacity_;
        data[end_] = val;
    }
    ++size_;

    DEQUE_CHECK(*this)
}


template<typename T>
T deque<T>::pop_back() {
    assert(size_);
    --size_;
    size_t return_pos = end_;
    end_ = (end_ - 1) & capacity_;

    #ifndef NDEBUG
    T tmp = data[return_pos];
    fillPoison(&data[return_pos]);
    return tmp;
    #else

    return data[return_pos];
    #endif

    DEQUE_CHECK(*this)
}


template<typename T>
void deque<T>::push_front(const T &val) {
    refit();
    if (!size_){
        data[0] = val;
    }
    else {
        begin_ = (begin_ - 1) & capacity_;
        data[begin_] = val;
    }
    ++size_;

    DEQUE_CHECK(*this)
}



template<typename T>
T deque<T>::pop_front() {
    assert(size_);
    --size_;
    size_t return_pos = begin_;
    begin_ = (begin_ + 1) & capacity_;

    #ifndef NDEBUG
    T tmp = data[return_pos];
    fillPoison(&data[return_pos]);
    return tmp;
    #else

    return data[return_pos];
    #endif

    DEQUE_CHECK(*this)
}


template<typename T>
void deque<T>::insert(size_t pos, const T& value) {
    size_t new_capacity = capacity_;
    if (size_ == capacity_ + 1)
        new_capacity = ((capacity_ + 1) << 1) - 1;
    T* new_data = new T[new_capacity + 1];

    #ifndef NDEBUG
    for (size_t k = 0; k <= new_capacity; ++k)
        fillPoison(&new_data[k]);
    #endif

    std::copy(begin(), begin() + pos, new_data);
    new_data[pos] = value;
    std::copy(begin() + pos, end(), new_data + pos + 1);

    begin_ = 0;
    end_ = size_;
    ++size_;
    capacity_ = new_capacity;
    if (data)
        delete[] data;
    data = new_data;

    DEQUE_CHECK(*this)
}


template<typename T>
T deque<T>::erase(size_t pos) {
    T* new_data = new T[capacity_ + 1];
    T result = data[pos];
    #ifndef NDEBUG
    for (size_t k = 0; k <= capacity_; ++k)
        fillPoison(&new_data[k]);
    #endif

    std::copy(begin(), begin() + pos, new_data);
    std::copy(begin() + pos + 1, end(), new_data + pos);

    begin_ = 0;
    end_ = size_ - 2;
    --size_;

    if (data)
        delete[] data;
    data = new_data;

    DEQUE_CHECK(*this)
    return result;
}


template<typename T>
void deque<T>::refit(size_t new_capacity) {
    if (new_capacity == -1 && ((size_ == capacity_ + 1) || capacity_ == 0))
        new_capacity = ((capacity_ + 1) << 1) - 1;
    else if (new_capacity == -1)
        return;
    else {
        size_t i = 2;

        while (i < new_capacity)
            i <<= 1;
        new_capacity = i - 1;
    }

    if (new_capacity == capacity_)
        return;

    T* new_data = new T[new_capacity + 1];

    #ifndef NDEBUG
    for (size_t k = 0; k <= new_capacity; ++k)
        fillPoison(&new_data[k]);
    #endif

    if (new_capacity < size_){
        std::copy(begin(), begin() + (new_capacity + 1), new_data);
        size_ = new_capacity + 1;
    }
    else
        std::copy(begin(), end(), new_data);

    begin_ = 0;
    if (size_)
        end_ = size_ - 1;
    else
        end_ = 0;
    capacity_ = new_capacity;
    delete[] data;
    data = new_data;

    DEQUE_CHECK(*this)
}


template<typename T>
bool deque<T>::operator==( const deque &other ) const {
    if (size_ != other.size_)
        return false;
    for (size_t i = 0; i < size_; ++i)
        if ((*this)[i] != other[i])
            return false;
    return true;
}


template<typename T>
template<typename U>
bool deque<T>::operator==( const U &other ) const {
    if (size_ != other.size())
        return false;
    for (size_t i = 0; i < size_; ++i)
        if ((*this)[i] != other[i])
            return false;
    return true;
}


template<typename T>
deque<T>& deque<T>::operator=(const deque &other){
    capacity_ = other.capacity_;
    begin_ = other.begin_;
    end_ = other.end_;
    size_ = other.size_;
    if (data)
        delete[] data;
    data = new T[capacity_ + 1];
    std::copy(other.data, other.data + capacity_ + 1, data);

    DEQUE_CHECK(*this)

    return *this;
}


template<typename T>
deque<T>& deque<T>::operator=(deque &&other){
    capacity_ = std::exchange(other.capacity_, 0);
    begin_    = std::exchange(other.begin_,    0);
    end_      = std::exchange(other.end_,      0);
    size_     = std::exchange(other.size_,     0);
    data      = std::exchange(other.data, nullptr);

    DEQUE_CHECK(*this)

    return *this;
}

} // namespace g
#endif
