#pragma once

#include <unordered_map>
#include "Packet.h"
#include "glog/logging.h"


constexpr int kBufferLen = 10000;
class PacketBuffer{
private:
    Packet* buffer;
    int current_index = 0;
    std::unordered_map<int, uint64_t> index_to_seq;
    std::unordered_map<uint64_t, int> seq_to_index;
public:
    PacketBuffer() {
        buffer = (Packet*)malloc(sizeof(Packet) * kBufferLen);
    };

    int GetALocation() {
        sizeof(buffer);
        int rtn = current_index;
        current_index = (current_index + 1) % kBufferLen;
        return rtn;
    }
    std::vector<int> GetLocations(int size) {
        std::vector<int> rtn(size);
        for (int i = 0; i < size; ++i) {
            rtn[i] = GetALocation();
        }
        return rtn;
    }
    uint64_t IndexToSeq(int index) const {
        auto iter = index_to_seq.find(index);
        if (iter == index_to_seq.end()) {
            LOG(ERROR) << "the index in the buffer is meaningless";
            return -1;
        }

        DCHECK_EQ(buffer[index].GetSequenceNum(), iter->second) << "buffer is wrong, index_to_seq error!";
        return iter->second;
    } 
    int SeqToIndex(uint64_t seq) const {
        auto iter = seq_to_index.find(seq);
        if (iter == seq_to_index.end()) {
            LOG(ERROR) << "the seq is not in the buffer";
            return -1;
        }
        return iter->second;
    }
    const Packet* FindPacket(int index) const {
        return &buffer[index];
    }
    const Packet* FindPacket(uint64_t seq) const {
        int index = SeqToIndex(seq);
        if (index != -1) {
            return &buffer[index];
        }
        return NULL;
    }
    void ClearIndex(int index) {
        auto index_iter = index_to_seq.find(index);
        if (index_iter != index_to_seq.end()) {
            uint64_t seq = index_iter->second;
            seq_to_index.erase(seq);
            index_to_seq.erase(index_iter);
        }
    }
    void ClearSeq(uint64_t seq) {
        auto seq_iter = seq_to_index.find(seq);
        if (seq_iter != seq_to_index.end()) {
            int index = seq_iter->second;
            index_to_seq.erase(index);
            seq_to_index.erase(seq_iter);
        }
    }
    void CacheAPacket(const Packet&p) {
        int index = GetALocation();
        ClearIndex(index);
        buffer[index] = p;
        seq_to_index[p.GetSequenceNum()] = index;
        index_to_seq[index] = p.GetSequenceNum();
    }
    int Size() const {
        return seq_to_index.size();
    }
    ~PacketBuffer() {
        if (buffer) {
            delete buffer;
            buffer = NULL;
        }
    }
};