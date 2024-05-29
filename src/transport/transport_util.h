#pragma once
#include "packet.pb.h"
#include "FEC/fec_util.h"
#include <string>

// 不会返回FEC_NONE
inline FecType RandomFecType() {
    int total_kinds = 15;
    return static_cast<FecType>(rand() % total_kinds + 1);
}

inline uint32_t RandomFecIndex(FecType type) {
    FecInfo info = GetInfoAboutFEC(type);
    return rand() % info.TotalCount();
}

inline uint64_t RandomSeqNum() {
    return rand() % UINT64_MAX;
}

inline DataPacket RandomDataPacket() {
    DataPacket p;
    int len = rand() % 1450 + 30;
    uint8_t* buffer = new uint8_t[len];
    for (int i = 0; i < len; ++i) {
        buffer[i] = rand() % UINT8_MAX;
    }
    p.set_data(buffer, len);
    p.set_len(len);
    delete[] buffer;
    return p;
}

inline Packet RandomPacket(uint64_t seq_num) {
    Packet p;

    DataPacket* d = new DataPacket;
    int len = rand() % 1400;
    std::string s;
    s.reserve(len);
    for (int i = 0; i < len; ++i) {
        char ch = static_cast<char>(rand() % UINT8_MAX);
        s.push_back(ch);
    }
    d->set_data(s);
    d->set_len(len);
    p.set_allocated_data_packet(d);

    FecType type = RandomFecType();
    p.set_fec_type(type);
    p.set_fec_index(RandomFecIndex(type));
    p.set_seq_num(seq_num);
    // p.set_subpacket_len(d->ByteSizeLong());
    // assert(p.subpacket_len() == d->ByteSizeLong());
    return p;
}

