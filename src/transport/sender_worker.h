#pragma once

#include "packet.pb.h"
#include "packet_cache.h"
#include "config.h"
#include "FEC/fec_codec.h"
#include "FEC/fec_util.h"
class SWorker{
private:
    PacketCache<Packet> cache_;
    Config config_;
    uint64_t seq_num_; // 下一个要发送的序列号
    BlockFecCodec codec_;
                    // timestamps  seq_num
    std::list<std::pair<uint64_t, uint64_t>> to_fec_encode_;
    uint8_t buffer_[kMaxPayloadLen * kMaxFecTotalPackets];

    void ClearFec(std::list<Packet*>& rtn);
    void EncodeFecOnce(std::list<Packet*>& rtn);
    Packet* RegisterDataPacket(const DataPacket& packet);
    Packet* RegisterDataPacket(void* buffer, int len);

    void HandleFeedBack(const Packet& packet, std::list<Packet*>&);
    void HandleProbe(const Packet& packet, std::list<Packet*>&);
public:
    SWorker(Config config);

    inline void SetConfig(const Config&);

    void RegisterPackets(const DataPacket& packet, std::list<Packet*>&);

    void HandleReceive(const Packet& packet, std::list<Packet*>&);
};