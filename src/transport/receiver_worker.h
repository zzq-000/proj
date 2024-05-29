#pragma once
#include <glog/logging.h>
#include "config.h"
#include "packet_cache.h"
#include "packet.pb.h"
#include "probe_stat.h"
#include "packet_logger.h"
#include "FEC/fec_codec.h"
#include "FEC/fec_util.h"

class RWorker {

private:
    static constexpr int kBufferSize = kMaxPayloadLen * kMaxFecTotalPackets;
    Config config_;
    PacketCache<Packet> cache_;
    BlockFecCodec codec_;
    uint64_t last_submit_seq_;
    uint64_t last_submit_time_;
    bool received_first_;
    uint8_t* buffer_;
public:
    RWorker(Config config);
    std::list<DataPacket*> GetApplicationMessages(const Packet& packet);
    std::list<Packet*> GetFeedback() const;
    ~RWorker();
};