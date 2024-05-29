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
    uint64_t next_submit_seq_;
    uint64_t last_submit_time_;
    bool received_first_;
    uint8_t buffer_[kMaxFecTotalPackets * kMaxPayloadLen];
public:
    RWorker(Config config);
    void GetApplicationMessages(const Packet& packet, std::list<DataPacket*>&);
    std::list<Packet*> GetFeedback() const;
};