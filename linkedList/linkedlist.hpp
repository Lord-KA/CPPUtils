#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <iostream>
#include <cassert>


namespace g {

template<class T, class U = T>
static T exchange(T& obj, U&& new_value)
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}

//==========================================
// Object pool

template<typename Data>
class ObjPool{
public:

    ObjPool(const ObjPool &other)
    {
        capacity = other.capacity;
        last_free = other.last_free;
        data = new Node[capacity];
        std::copy(other.data, other.data + other.capacity, data);
    }

    ObjPool(ObjPool &&other)
    {
        capacity = exchange(other.capacity, 0);
        last_free = exchange(other.last_free, -1);
        data = exchange(other.data, nullptr);
    }

    ObjPool& operator=(const ObjPool &other)
    {
        capacity = other.capacity;
        last_free = other.last_free;
        if (data)
            delete[] data;
        data = new Node[capacity];
        std::copy(other.data, other.data + other.capacity, data);
        return (*this);
    }

    ObjPool& operator=(ObjPool &&other)
    {
        capacity = exchange(other.capacity, 0);
        last_free = exchange(other.last_free, -1);
        if (data)
            delete[] data;
        data = exchange(other.data, nullptr);
        return (*this);
    }


    ObjPool(size_t capacity=1) : capacity(capacity)
    {
        data = new Node [capacity];
        for (size_t i=0; i<capacity - 1; ++i)
            data[i].next = i + 1;
        data[capacity - 1].next = -1;

        last_free = 0;
    }
    ~ObjPool()
    {
        delete [] data;
    }

    size_t alloc()
    {
        refit();
        size_t result = last_free;
        last_free = data[last_free].next;
        return result;
    }

    Data *get(size_t id) const
    {
        assert(id != -1);
        assert(id < capacity);
        return &data[id].val;
    }

    void free(size_t id)
    {
        data[id].next = last_free;
        last_free = id;
    }

    void print(std::ostream& out)
    {
        for (size_t id = last_free; id != -1; id = data[id].next)
        {
            out << "(" << id << ") -> ";
        }
        out << '\n';
    }


private:
    struct Node{
        size_t next;
        Data val;
    };

    Node *data;
    size_t capacity;
    size_t last_free;

    void refit()
    {
        if (last_free != -1)
            return;
        Node *nbuf = new Node[capacity * 2];
        std::copy(data, data + capacity, nbuf);
        assert(data);
        delete [] data;
        data = nbuf;
        capacity *= 2;
        for (size_t i = capacity / 2; i < capacity - 1; ++i)
            data[i].next = i + 1;
        data[capacity - 1].next = -1;
        last_free = capacity / 2;
    }
};

//==========================================
// LinkedList

template<typename T>
class list {
public:
    struct Node{
        size_t next_;
        T val_;
    };

private:
    size_t head_;
    size_t size_;
    ObjPool<Node> pool;

public:
    //===================================
    //  Interface functions

    list() : head_(-1), size_(0) {}
    list( const list &other ) = default;
    list( list &&other ) : head_(other.head_), size_(other.size_) { pool = std::move(other.pool); other.head_ = -1; other.size_ = 0; }

    list& operator=( const list &other ) { head_ = other.head_; size_ = other.size_; pool = other.pool; return *this; }
    list& operator=( list &&other )  { head_ = other.head_; size_ = other.size_; pool = std::move(other.pool); other.head_ = -1; other.size_ = 0; return *this; }


    void insert( const T &val ) { insert(0, val); };
    void insert( size_t n, const T &val );
    T erase( size_t n = 0 );

    size_t size() const { return size_; }

    template<class Container>
    bool operator==( const Container &other ) const;
    template<class Container>
    bool operator!=( const Container &other ) const { return !(*this == other); }

    void dump(std::ostream &out) const;

    T& operator[](size_t n);
    const T& operator[](size_t n) const;

    //===================================
    //  Iterators

    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Node;

        Iterator( size_t id = -1, const ObjPool<Node> *pool = nullptr ) : pool_(pool), id_(id) {};
        Iterator( const Iterator &other ) = default;

        bool operator==( const Iterator &other ) const { return id_ == other.id_; }
        bool operator!=( const Iterator &other ) const { return id_ != other.id_; }

        T operator*() { assert(id_ != -1); return pool_->get(id_)->val_; }
        const T operator*() const { assert(id_ != -1); return pool_->get(id_)->val_; }

        Iterator operator++() {
            id_ = pool_->get(id_)->next_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator result(*this);
            id_ = pool_->get(id_)->next_;
            return result;
        }


    private:
        const ObjPool<Node> *pool_;
        size_t id_;

    };

    Iterator begin() const { return Iterator(head_, &pool); }
    Iterator end()   const { return Iterator(   -1, &pool); }
};

template<typename T>
void list<T>::insert(size_t n, const T &val) {
    assert(n <= size_);
    ++size_;
    if (n == 0){
        size_t id = pool.alloc();
        pool.get(id)->next_ = head_;
        pool.get(id)->val_ = val;
        head_ = id;
        return;
    }
    size_t id = head_;
    for (size_t i = 0; i < n - 1; ++i)
        id = pool.get(id)->next_;

    size_t new_id = pool.alloc();
    Node *v = pool.get(id), *new_ = pool.get(new_id);
    new_->next_ = v->next_;
    new_->val_ = val;
    v->next_ = new_id;
}

template<typename T>
T list<T>::erase(size_t n) {
    assert(n < size_);
    --size_;
    if (n == 0){
        Node *v = pool.get(head_);
        head_ = v->next_;
        return v->val_;
    }
    size_t id = head_;
    for (size_t i = 0; i < n - 1; ++i)
        id = pool.get(id)->next_;
    Node *v = pool.get(id);
    size_t result_id = v->next_;
    v->next_ = pool.get(result_id)->next_;
    T result_val = pool.get(result_id)->val_;
    pool.free(result_id);
    return result_val;
}

template<typename T>
void list<T>::dump(std::ostream &out) const {
    out << "head = " << head_ << '\n';
    out << "size = " << size_ << '\n';
    for (auto elem : *this)
        out << "( " << elem << " ) ";
    out << '\n';
}

template<typename T>
template<class Container>
bool list<T>::operator==(const Container &other) const {
    if (size() != other.size()){
        return false;
    }

    Iterator iter_1 = begin();
    auto iter_2 = other.begin();
    while (iter_1 != end()) {
        if (*iter_1 != *iter_2)
            return false;
        ++iter_1; ++iter_2;
    }
    return true;
}


template<typename T>
T& list<T>::operator[](size_t n) {
    size_t id = head_;
    for (size_t i = 0; i < n; ++i)
        id = pool.get(id)->next_;
    return pool.get(id)->val_;
}

template<typename T>
const T& list<T>::operator[](size_t n) const {
    size_t id = head_;
    for (size_t i = 0; i < n; ++i)
        id = pool.get(id)->next_;
    return pool.get(id)->val_;
}

} // namespace g
#endif
