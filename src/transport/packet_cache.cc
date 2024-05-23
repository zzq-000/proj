#include "packet_cache.h"
#include "packet.pb.h"
template<typename CachedType>
PacketCache<CachedType>::PacketCache(uint32_t size):size_(size), next_index_(0){
    static_assert(has_member_function<CachedType, uint64_t(CachedType::*)() const, &CachedType::seq_num>::value);
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
    return *this;
}

template<typename CachedType>
PacketCache<CachedType>::~PacketCache() {
    if (data_ != NULL) {
        delete [] data_;
    }
}

template<typename CachedType>
CachedType* PacketCache<CachedType>::FindPacket(uint64_t seq_num) {
    int previous_index = (next_index_ - 1 + size_) % size_;


    int delta = seq_num - data_[previous_index].seq_num();

    // 这里如果不转换, delta会被隐式转换为uint32, 会有问题
    int tmp_size = (int)size_;

    int target_index = (previous_index + (delta % tmp_size + tmp_size)) % tmp_size;

    if (data_[target_index].seq_num() == seq_num) {
        return data_ + target_index;
    }
    return NULL;
}

template<typename CachedType>
void PacketCache<CachedType>::CachePacket(const CachedType& packet) {
    data_[next_index_].CopyFrom(packet);
    next_index_ = (next_index_ + 1) % size_;
}

template class PacketCache<Packet>;