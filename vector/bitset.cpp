#include <cstdint>
#include <iostream>
#include <cassert>

class bitset {
    static const size_t CAPACITY = 1<<30;

    private:

    struct bool_reference {

        bool_reference(bitset &base, size_t pos) : base_(base), pos_(pos) {}

        bool_reference& operator=(bool val)
        {
            base_.raw_set(pos_, val);
            return *this;
        }

        bool_reference& operator=(bool_reference &other)
        {
            assert(&other.base_ == &base_);
            pos_ = other.pos_;
            return *this;
        }

        operator bool()
        {
            return base_.raw_get(pos_);
        }

        bool_reference& operator^=(bool_reference &other)
        {
            base_.raw_set(pos_, base_.raw_get(pos_) ^ other.base_.raw_get(other.pos_));
            return *this;
        }

        bool_reference& operator^=(bool val)
        {
            base_.raw_set(pos_, base_.raw_get(pos_) ^ val);
            return *this;
        }

    private:
        bitset &base_;
        size_t pos_ = -1;
    };

    struct const_bool_reference {

        const_bool_reference(const bitset &base, size_t pos) : base_(base), pos_(pos) {}

        operator bool()
        {
            return base_.raw_get(pos_);
        }

    private:
        const bitset &base_;
        const size_t pos_ = -1;
    };

    public:
        bitset()
        {
            data_ = new uint64_t[CAPACITY / 64];
            assert(data_);
        }
        ~bitset()
        {
            assert(data_ != nullptr);
            delete[] data_;
        }

        bool_reference operator[](size_t pos)
        {
            assert(pos < size_);
            return bool_reference(*this, pos);
        }

        const_bool_reference operator[](size_t pos) const
        {
            assert(pos < size_);
            return const_bool_reference(*this, pos);
        }

        void push_back(bool val)
        {
            assert(size_ < CAPACITY);
            ++size_;
            raw_set(size_ - 1, val);
        }

        size_t size() const
        {
            return size_;
        }

        void raw_set(size_t pos, bool val)
        {
            assert(pos < size_);
            if (val)
                data_[pos / 64] |= 1ULL << (pos % 64);
            else
                data_[pos / 64] &=  ~(1ULL << (pos % 64));
        }

        bool raw_get(size_t pos) const
        {
            assert(pos < size_);
            return data_[pos / 64] & (1ULL << (pos % 64));
        }

    private:
        uint64_t *data_ = nullptr;
        size_t size_ = 0;
};



int main()
{
    bitset b;
    b.push_back(0);
    b.push_back(1);
    b.push_back(0);
    b.push_back(0);
    b.push_back(0);
    b.push_back(1);
    b.push_back(1);
    b.push_back(0);
    for (size_t i = 0; i < 12000; ++i)
        b.push_back(1);
    b.push_back(0);
    b.push_back(1);
    b.push_back(0);
    std::cout << "size = " << b.size() << "\n";
    for (size_t i = 0; i < 100000000; ++i) {
        size_t pos = i % b.size();
        //b[pos] ^= 1;
        b.raw_set(pos, b.raw_get(pos) ^ 1);
    }
    std::cout << "\n";

}
