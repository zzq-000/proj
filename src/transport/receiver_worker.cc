#include "receiver_worker.h"


RWorker::RWorker(Config config): config_(config), cache_(), codec_() {}

std::list<DataPacket*> RWorker::GetApplicationMessages(const Packet& packet) {
    std::list<DataPacket*> rtn;
    cache_.CachePacket(packet);
    if (packet.fec_type() == FecType::FEC_NONE) {
        Packet* p = cache_.FindPacket(packet.seq_num());
        if (p != NULL) {
            rtn.push_back(const_cast<DataPacket*>(&(p->data_packet())));
        }
    }

}