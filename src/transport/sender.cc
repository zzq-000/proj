#include "sender.h"

std::vector<Datagram> Sender::RegisterMessages(void* buffer, int len) {
    // Packet* p = reinterpret_cast<Packet*>(buffer);
    int index = packets.CacheAPacket(buffer, len);
    uint64_t ts = timestamp_ms();
    packets_to_send.emplace_back(index, ts);

    if (config.enable_fec && config.fec_type != FecType::Fec_none) {
        return HandleFecPackets(ts);
    }
    return HandleFecTypeNonePackets(ts);
}
std::vector<Datagram> Sender::HandleFecPackets(uint64_t ts) {
    bool need_send = (ts - packets_to_send.front().second > config.fec_encode_wait_time_limit_ms);
    if (!need_send) return {};

    std::vector<Datagram> rtn;

    FecInfo info = GetInfoAboutFEC(config.fec_type);

    // 如果packet_to_send < info.data_cnt ?  用一些空包补足data_cnt, 接收端接受到空包时，无法提取数据，会被过滤掉
    int data_cnt = std::min((int)packets_to_send.size(), info.data_cnt);
    int redundancy_cnt = info.TotalCount() - data_cnt;

    std::vector<int> redundant_indices = packets.GetLocations(info.redundancy_cnt);
    for (auto index : redundant_indices) {
        packets.FindPacket(index)->ClearDataPacket();
    }
    std::vector<int> indices(info.TotalCount());
    for (int i = 0; i < data_cnt; ++i) {
        auto& [index, _] = packets_to_send.front();
        packets_to_send.pop_front();
        indices[i] = index;
    }
    for (int i = data_cnt; i < info.TotalCount(); ++i) {
        indices[i] = redundant_indices[i - data_cnt];
    }
    int encode_size = packets.FindPacket(indices[0])->GetPacketSize();
    std::vector<void*> encode_data(info.TotalCount());
    for (int i = 1; i < indices.size(); ++i) {
        encode_size = std::max(encode_size, (int)packets.FindPacket(indices[i])->GetPacketSize());
        encode_data[i] = packets.FindPacket(indices[i]);
    }
    fec_codec.Encode(encode_data, config.fec_type, encode_size);

    for (int i = 0; i < indices.size(); ++i) {
        Packet* p = packets.FindPacket(indices[i]);
        InitPacket(p);
        p->SetPacketType(PacketType::DataPacket);
        p->SetFecIndex(i);
        rtn.emplace_back(p, p->GetPacketSize());
    }

    return rtn;

}

std::vector<Datagram> Sender::HandleFecTypeNonePackets(uint64_t ts) {
    std::vector<Datagram> rtn;
    while (!packets_to_send.empty()) {
        auto& [index, push_time] = packets_to_send.front();
        packets_to_send.pop_front();
        Packet* p = packets.FindPacket(index);
        p->SetPacketType(PacketType::DataPacket);
        rtn.emplace_back(p, p->GetPacketSize());
    }
    return rtn;
} 

void Sender::InitPacket(Packet* p) {
    p->SetSequenceNum(seq_num++);
    p->SetFecType(config.fec_type);
}