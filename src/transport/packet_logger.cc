#include "packet_logger.h"
#include "util/timestamp.h"
#include "packet.pb.h"
#include "glog/logging.h"
PacketLogger::PacketLogger()
    : loss_stat_(), window_left_seq_num_(0), clean_seq_duration_ms_(kCleanDurationMs), size_(0) {
    cache_ = new PacketRecord[kRecordsNum];
}
inline int PacketLogger::GetSize() const {
        return size_;
}


void PacketLogger::InitRecord(uint64_t seq_num, uint64_t timestamp_ms, uint32_t arq_wait_time_ms) {
    int index = SeqNumToIndex(seq_num);
    cache_[index].seq_num = seq_num;
    cache_[index].register_time = timestamp_ms;
    cache_[index].processed = false;
    cache_[index].received = false;
    cache_[index].arq_count = 0;
    cache_[index].is_redundant = false;
    cache_[index].retransmit_time = timestamp_ms + arq_wait_time_ms;
}

int PacketLogger::SeqNumToIndex(uint64_t seq_num) const {
    return seq_num % kRecordsNum;
}

void PacketLogger::SetReceived(uint64_t seq_num) {
    int index = SeqNumToIndex(seq_num);
    if (cache_[index].seq_num == seq_num) {
        cache_[index].received = true;
    }
} 
void PacketLogger::SetProcessed(uint64_t seq_num) {
    int index = SeqNumToIndex(seq_num);
    if (cache_[index].seq_num == seq_num) {
        cache_[index].processed = true;
    }
}

void PacketLogger::ReclaimSeq() {
    uint64_t now = timestamp_ms();
    uint64_t window_right_seq = window_left_seq_num_ + size_;
    while (window_left_seq_num_ < window_right_seq) {
        int index = SeqNumToIndex(window_left_seq_num_);
        DCHECK_EQ(cache_[index].seq_num, window_left_seq_num_);
        if (now - cache_[index].register_time > clean_seq_duration_ms_) {
            loss_stat_.RegisterAPacket(cache_[index].register_time, cache_[index].received,
                cache_[index].processed, cache_[index].is_redundant);
            window_left_seq_num_++;
            size_ --;
        }else {
            break;
        }
    }

}

// 1. 清理过期的序列号
// 1. 注册新序列号
// 2. 设置Received
void PacketLogger::ReceivePacket(const Packet& packet, uint32_t arq_wait_time_ms) {
    // LOG(INFO) << "start seq: " << window_left_seq_num_;
    ReclaimSeq();
    uint64_t seq_num = packet.seq_num();
    uint64_t now = timestamp_ms();
    if (size_ == 0) {
        window_left_seq_num_ = seq_num;
        InitRecord(seq_num, now, arq_wait_time_ms);
        size_ = 1;        
    }

    // 该范围的序列号已经过时， 即便此时收到了， 也不满足实时性的要求， 不需要考虑
    if (seq_num < window_left_seq_num_) {
        return;
    }else if (seq_num < window_left_seq_num_ + size_) {
        // 该范围内的序列号已经被注册
    }else {
        // 扩大接收窗口
        for (uint64_t seq = window_left_seq_num_ + size_; seq <= seq_num; seq++) {
            InitRecord(seq, now, arq_wait_time_ms);
            size_++;
        }
    }
    SetReceived(seq_num);
}



// 包括冗余包在内的丢包率， 数据包的丢包率
std::pair<double, double> PacketLogger::GetLossRate() const {
    return loss_stat_.GetLastMiniteLossRate(timestamp_ms());
}




PacketLogger::~PacketLogger() {
    if (cache_) {
        delete [] cache_;
        cache_ = nullptr;
    }
}