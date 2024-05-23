#pragma once

#include "has_member_function.h"
#include <cstddef>
#include <cstdint>

template<typename CachedType>
class PacketCache{
private:
    CachedType* data_;
    uint32_t size_;
    int next_index_; // 存放下一个包应该存放的索引
public:
    explicit PacketCache(uint32_t size);
    PacketCache(const PacketCache& rhs) = delete;
    PacketCache& operator=(const PacketCache& rhs) = delete;
    PacketCache(PacketCache&& rhs);
    PacketCache& operator=(PacketCache&& rhs);
    ~PacketCache();


    CachedType* FindPacket(uint64_t seq_num);
    void CachePacket(const CachedType& packet);
};