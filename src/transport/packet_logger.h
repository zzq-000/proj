#include <cstdint>
#include "packet.h"

constexpr int kRecordLen = 10000;

class PacketRecord{
    uint64_t seq_num;
    int arq_count;
    bool received;
    bool processed;

};

class PacketLogger{
private:
    PacketRecord* cache;
    uint64_t start_seq_num;
    int size;
public:
    PacketLogger(): start_seq_num(0), size(0) {
        cache = new PacketRecord[kRecordLen];
    }
    inline int GetSize() const {
        return size;
    }
    void ReceivePacket(const Packet* p) const;
    double GetLossRate() const;


    ~PacketLogger() {
        if (cache) {
            delete [] cache;
            cache = nullptr;
        }
    }
};