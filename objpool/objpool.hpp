#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <exception>
#include <iostream>
#include <new>
#include <sys/types.h>
#include <vector>
#include <utility>

namespace g {

template<typename Data>
class ObjPool{

private:

    enum class NodeState : uint8_t {
        Free,
        Allocated,
        Deallocated,
    };

    struct PoolId {
        PoolId(size_t id)
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

    struct Node{
        NodeState state;
        PoolId next;
        Data val;
    };

public:
    explicit ObjPool(size_t capacity = 16) :
        capacity_(capacity),
        free_count_(capacity),
        last_free_(0)
    {
        slab_sizes_.push_back(capacity);

        Node *data = (Node*)calloc(capacity, sizeof(Node));
        if (data == nullptr)
            throw std::bad_alloc();

        for (size_t i = 0; i < capacity - 1; ++i)
            data[i].next = PoolId(0, i + 1);
        data[capacity - 1].next = -1;

        slabs_.push_back(data);
    }

    ObjPool(ObjPool &&other) :
        slabs_(std::move(other.slabs_)),
        slab_sizes_(std::move(other.slab_sizes_)),
        capacity_(other.capacity_),
        free_count_(other.free_count_),
        last_free_(other.last_free_)
    {
        other.capacity_ = 0;
        other.free_count_ = 0;
        other.last_free_ = -1;
    }

    ObjPool& operator=(ObjPool &&other)
    {
        for (size_t i = 0; i < slabs_.size(); ++i) {
            for (size_t j = 0; j < slab_sizes_[i]; ++j) {
                Node &node = slabs_[i][j];
                if (node.state == NodeState::Allocated)
                    node.val.~Data();
            }
            free(slabs_[i]);
        }

        slabs_ = std::move(other.slabs_);
        slab_sizes_ = std::move(other.slab_sizes_);
        capacity_ = other.capacity_;
        free_count_ = other.free_count_;
        last_free_ = other.last_free_;

        other.capacity_ = 0;
        other.free_count_ = 0;
        other.last_free_ = -1;
    }

    ~ObjPool()
    {
        for (size_t i = 0; i < slabs_.size(); ++i) {
            for (size_t j = 0; j < slab_sizes_[i]; ++j) {
                Node &node = slabs_[i][j];
                if (node.state == NodeState::Allocated)
                    node.val.~Data();
            }
            free(slabs_[i]);
        }
        capacity_ = 0;
        free_count_ = 0;
        last_free_ = -1;
    }


    size_t Alloc()
    {
        if (free_count_ == 0)
            Expand();

        Node &node = GetNode(last_free_);
        node.state = NodeState::Allocated;

        --free_count_;
        return std::exchange(last_free_, node.next).toNum();
    }

    template<typename ...Args>
    size_t InitAlloc(Args ...args)
    {
        size_t id = Alloc();
        new(&Get(id)) Data(args...);
        return id;
    }

    Data& Get(PoolId id) const
    {
        return GetNode(id).val;
    }

    void Free(PoolId id)
    {
        ++free_count_;
        Get(id).~Data();
        Node &node = GetNode(id);
        node.state = NodeState::Deallocated;
        node.next = last_free_;
        last_free_ = id;
    }

private:
    std::vector<Node*> slabs_;
    std::vector<size_t> slab_sizes_;
    size_t capacity_;
    size_t free_count_;
    PoolId last_free_;

    Node& GetNode(PoolId id) const
    {
        assert(id.slab_id_ < slabs_.size());
        assert(id.node_id_ < slab_sizes_[id.slab_id_]);
        return slabs_[id.slab_id_][id.node_id_];
    }

    void Expand()
    {
        if (free_count_ > 0)
            return;

        Node *data = (Node*)calloc(capacity_ * 2, sizeof(Node));
        size_t data_size;
        size_t slab_id = slabs_.size();

        if (data == nullptr) {
            data = (Node*)calloc(capacity_, sizeof(Node));
            if (data == nullptr)
                throw std::bad_alloc();
            data_size = capacity_;
        } else {
            data_size = capacity_ * 2;
        }

        last_free_ = PoolId(slab_id, 0);
        slabs_.push_back(data);
        slab_sizes_.push_back(data_size);
        capacity_ += data_size;
        free_count_ += data_size;

        for (size_t i = 0; i < data_size - 1; ++i)
            data[i].next = PoolId(slab_id, i + 1);
        data[data_size - 1].next = -1;
    }
};

} /* namespace g */
