#include <cstdint>
#include "packet.pb.h"
#include "probe_stat.h"

class PacketRecord{
public:
    uint64_t seq_num;
    uint64_t register_time;
    uint64_t retransmit_time;
    int arq_count;
    bool is_redundant;
    bool received;
    bool processed;

};

class PacketLogger{
public:
    static constexpr int kRecordsNum = 10000;
    static constexpr uint64_t kCleanDurationMs = 500;
private:
    PacketRecord* cache_;
    LossStat loss_stat_;
    uint64_t window_left_seq_num_;
    uint64_t clean_seq_duration_ms_;
    int size_;

    void InitRecord(uint64_t seq_num, uint64_t timestamp_ms, uint32_t arq_wait_time_ms); 
    int SeqNumToIndex(uint64_t seq_num) const;
    void ReclaimSeq();
    // 冗余包一定不会提交给应用层
    // 接收到， 但不一定提交给应用层， 该接口呀由ReceivePacket调用
    void SetReceived(uint64_t seq_num); 
public:
    PacketLogger();
    PacketLogger(const PacketLogger& rhs) = delete;
    PacketLogger& operator= (const PacketLogger& rhs) = delete;
    PacketLogger(PacketLogger&& rhs) = delete;
    PacketLogger& operator= (PacketLogger&& rhs) = delete;
    
    inline int GetSize() const;
    void ReceivePacket(const Packet& p, uint32_t arq_wait_time_ms);
    
    // 提交给应用层， 将数据包提取出来给应用层的时候， 由使用者调用
    void SetProcessed(uint64_t seq_num);
    std::pair<double, double> GetLossRate() const;


    ~PacketLogger();
};