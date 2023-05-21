#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <utility>

namespace g {

class MemPool{

private:

    struct PoolId {
        PoolId(size_t id = -1)
        {
            static_assert(sizeof(PoolId) == sizeof(size_t), "PoolId must be 8 bytes");
            *this = *(PoolId*)(&id);
        }

        PoolId(uint32_t slab, uint32_t node)
            : slab_id_(slab), node_id_(node) {}

        size_t toNum()
        {
            return *(size_t*)(this);
        }

        uint32_t slab_id_;
        uint32_t node_id_;
    };

public:
    explicit MemPool(size_t objsize, size_t capacity = 16) :
        free_nodes_(capacity),
        objsize_(objsize),
        capacity_(capacity),
        free_count_(capacity)
    {
        assert(objsize != 0);
        slab_sizes_.push_back(capacity);

        uint8_t *data = (uint8_t*)calloc(capacity, objsize_);
        if (data == nullptr)
            throw std::bad_alloc();

        for (size_t i = 0; i < capacity; ++i)
            free_nodes_[i] = PoolId(0, i);

        slabs_.push_back(data);
    }

    MemPool(MemPool &&other) :
        slabs_(std::move(other.slabs_)),
        slab_sizes_(std::move(other.slab_sizes_)),
        free_nodes_(std::move(other.free_nodes_)),
        objsize_(other.objsize_),
        capacity_(other.capacity_),
        free_count_(other.free_count_)
    {
        other.objsize_ = -1;
        other.capacity_ = 0;
        other.free_count_ = 0;
    }

    MemPool& operator=(MemPool &&other)
    {
        for (uint8_t *slab : slabs_)
            free(slab);

        slabs_ = std::move(other.slabs_);
        slab_sizes_= std::move(other.slab_sizes_);
        free_nodes_ = std::move(other.free_nodes_);
        objsize_ = other.objsize_;
        capacity_ = other.capacity_;
        free_count_ = other.free_count_;

        other.objsize_ = -1;
        other.capacity_ = 0;
        other.free_count_ = 0;

        return *this;
    }

    ~MemPool()
    {
        for (uint8_t *slab : slabs_)
            free(slab);
        objsize_ = -1;
        capacity_ = 0;
        free_count_ = 0;
    }

    void* allocate(size_t n)
    {
        assert(n == objsize_);
        if (free_count_ == 0)
            Expand();

        --free_count_;
        PoolId pos = free_nodes_.back();
        free_nodes_.pop_back();
        return slabs_[pos.slab_id_] + pos.node_id_ * objsize_;
    }

    void deallocate(void *ptr)
    {
        ++free_count_;
        auto slab = std::lower_bound(slabs_.begin(), slabs_.end(), (uint8_t*)ptr) - 1;
        size_t slab_pos = slab - slabs_.begin();
        size_t node_pos = ((uint8_t*)ptr - *slab) / objsize_;

        if (node_pos >= slab_sizes_[slab_pos])
            throw std::out_of_range("attempt to free pointer outside of MemPool");

        free_nodes_.push_back(PoolId(slab_pos, node_pos));
    }

private:
    std::vector<uint8_t*> slabs_;
    std::vector<size_t> slab_sizes_;
    std::vector<PoolId> free_nodes_;
    size_t objsize_;
    size_t capacity_;
    size_t free_count_;

    void Expand()
    {
        if (free_count_ > 0)
            return;

        uint8_t *data = (uint8_t*)calloc(capacity_ * 2, objsize_);
        size_t data_size;
        size_t slab_id = slabs_.size();

        if (data == nullptr) {
            data = (uint8_t*)calloc(capacity_, objsize_);
            if (data == nullptr)
                throw std::bad_alloc();
            data_size = capacity_;
        } else {
            data_size = capacity_ * 2;
        }

        slabs_.push_back(data);
        slab_sizes_.push_back(data_size);
        capacity_ += data_size;
        free_count_ += data_size;
        free_nodes_.resize(free_nodes_.size() + data_size);

        for (size_t i = 0; i < data_size; ++i)
            free_nodes_[i + free_nodes_.size() - data_size] = PoolId(slab_id, i);
    }
};

} /* namespace g */
