#pragma once
#include <glog/logging.h>
#include "config.h"
#include "packet_cache.h"
#include "packet.pb.h"
#include "FEC/fec_codec.h"
#include "FEC/fec_util.h"

class RWorker {

private:
    Config config_;
    PacketCache<Packet> cache_;
    BlockFecCodec codec_;

public:
    RWorker(Config config);
    std::list<DataPacket*> GetApplicationMessages(const Packet& packet);
};