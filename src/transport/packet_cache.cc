#include "packet_cache.h"
#include "packet.pb.h"
template<typename CachedType>
PacketCache<CachedType>::PacketCache(uint32_t size):query_table_(), size_(size), next_index_(0){
    static_assert(has_member_function<CachedType, uint64_t(CachedType::*)() const, &CachedType::seq_num>::value);
    static_assert(has_member_function<CachedType, void(CachedType::*)(const CachedType&), &CachedType::CopyFrom>::value);
    data_ = new CachedType[size_];
}

template<typename CachedType>
PacketCache<CachedType>::PacketCache(PacketCache&& rhs)
    :data_(rhs.data_), query_table_(rhs.query_table_), size_(rhs.size_), next_index_(rhs.next_index_){

    rhs.data_ = NULL;
    rhs.size_ = 0;
    rhs.next_index_ = 0;
}

template<typename CachedType>
PacketCache<CachedType>& PacketCache<CachedType>::operator=(PacketCache<CachedType>&& rhs) {
    if (this != &rhs) {
        if (data_ != NULL) {
            delete [] data_;
        }
        data_ = rhs.data_;
        query_table_ = rhs.query_table_;
        size_ = rhs.size_;
        next_index_ = rhs.next_index_;

        rhs.data_ = NULL;
        rhs.query_table_.clear();
        rhs.size_ = 0;
        rhs.next_index_ = 0;
    }
    return *this;
}

template<typename CachedType>
PacketCache<CachedType>::~PacketCache() {
    if (data_ != NULL) {
        delete [] data_;
    }
}

template<typename CachedType>
CachedType* PacketCache<CachedType>::FindPacket(uint64_t seq_num) const{
    std::map<uint64_t, int>::const_iterator iter = query_table_.find(seq_num);
    if (iter == query_table_.cend() || data_[iter->second].seq_num() != seq_num) {
        return NULL;
    }
    return data_ + iter->second;
    // int previous_index = (next_index_ - 1 + size_) % size_;


    // int delta = seq_num - data_[previous_index].seq_num();

    // // 这里如果不转换, delta会被隐式转换为uint32, 会有问题
    // int tmp_size = (int)size_;

    // int target_index = (previous_index + (delta % tmp_size + tmp_size)) % tmp_size;

    // if (data_[target_index].seq_num() == seq_num) {
    //     return data_ + target_index;
    // }
    // return NULL;
}

template<typename CachedType>
void PacketCache<CachedType>::Shrink() {
    while (query_table_.size() > kThreshold * size_) {
        auto it = query_table_.begin();
        query_table_.erase(it);
    }
}

template<typename CachedType>
CachedType* PacketCache<CachedType>::GetNextPlaceToCache(uint64_t seq_num) {
    query_table_[seq_num] = next_index_;
    Shrink();
    CachedType* ans = data_ + next_index_;
    next_index_ = (next_index_ + 1) % size_;
    return ans;
}

template<typename CachedType>
void PacketCache<CachedType>::CachePacket(const CachedType& packet) {
    CachedType* find_packet = FindPacket(packet.seq_num());
    if (find_packet == NULL) {
        CachedType* p = GetNextPlaceToCache(packet.seq_num());
        p->CopyFrom(packet);
    }
}



template class PacketCache<Packet>;