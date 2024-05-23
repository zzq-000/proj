#pragma once
#include "packet.pb.h"
#include "FEC/fec_util.h"
#include <string>

inline FecType RandomFecType() {
    int total_kinds = 16;
    return static_cast<FecType>(rand() % total_kinds);
}

inline uint32_t RandomFecIndex(FecType type) {
    FecInfo info = GetInfoAboutFEC(type);
    return rand() % info.TotalCount();
}

inline uint64_t RandomSeqNum() {
    return rand() % UINT64_MAX;
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
    p.set_allocated_data_packet(d);

    FecType type = RandomFecType();
    p.set_fec_type(type);
    p.set_fec_index(RandomFecIndex(type));
    p.set_seq_num(seq_num);
    
    return p;
}

