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
    uint64_t seq_num_;
    BlockFecCodec codec_;
                    // timestamps  seq_num
    std::list<std::pair<uint64_t, uint64_t>> to_fec_encode_;

    void ClearFec(std::list<Packet*>& rtn);
    void EncodeFecOnce(std::list<Packet*>& rtn);
    Packet* RegisterDataPacket(const DataPacket& packet);
    Packet* RegisterDataPacket(void* buffer, int len);

    std::list<Packet*> HandleFeedBack(const Packet& packet);
    std::list<Packet*> HandleProbe(const Packet& packet);
public:
    SWorker(Config config);


    std::list<Packet*> RegisterPackets(const DataPacket& packet);

    std::list<Packet*> HandleReceive(const Packet& packet);
};