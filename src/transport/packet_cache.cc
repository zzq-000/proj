#include "packet_cache.h"

template<typename CachedType>
PacketCache<CachedType>::PacketCache(uint32_t size):size_(size), next_index_(0){
    static_assert(has_member_function<CachedType, uint32_t(CachedType::*)(), &CachedType::GetSeqNum>::value);
    data_ = new CachedType[size_];
}

template<typename CachedType>
PacketCache<CachedType>::PacketCache(PacketCache&& rhs)
    :data_(rhs.data_), size_(rhs.size_), next_index_(rhs.next_index_){

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
        size_ = rhs.size_;
        next_index_ = rhs.next_index_;

        rhs.data_ = NULL;
        rhs.size_ = 0;
        rhs.next_index_ = 0;
    }

}

template<typename CachedType>
PacketCache<CachedType>::~PacketCache() {
    if (data_ != NULL) {
        delete [] data_;
    }
}

template<typename CachedType>
CachedType* PacketCache<CachedType>::FindPacket(uint32_t seq_num) {
    int previous_index = (next_index_ - 1 + size_) % size_;


    int delta = seq_num - data_[previous_index].GetSeqNum();

    int target_index = (previous_index + (delta % size_ + size_)) % size_;

    if (data_[target_index].GetSeqNum() == seq_num) {
        return data_[target_index];
    }
    return NULL;
}

template<typename CachedType>
void PacketCache<CachedType>::CachePacket(const CachedType& packet) {
    data_[next_index_] = packet;
    next_index_ = (next_index_ + 1) % size_;
}
