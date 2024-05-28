#pragma once

#include "has_member_function.h"
#include <cstddef>
#include <cstdint>
#include <map>

template<typename CachedType>
class PacketCache{
private:
    CachedType* data_;
    std::map<uint64_t, int> query_table_;
    uint32_t size_;
    int next_index_; // 下一个包应该存放的索引
    void Shrink();
public:
    static constexpr int kThreshold = 3;
    explicit PacketCache(uint32_t size = 1000);
    PacketCache(const PacketCache& rhs) = delete;
    PacketCache& operator=(const PacketCache& rhs) = delete;
    PacketCache(PacketCache&& rhs);
    PacketCache& operator=(PacketCache&& rhs);
    ~PacketCache();


    CachedType* FindPacket(uint64_t seq_num) const;
    CachedType* GetNextPlaceToCache(uint64_t seq_num);

    void CachePacket(const CachedType& packet);

};