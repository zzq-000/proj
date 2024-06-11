#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <cmath>
#include <glog/logging.h>
class ProbeStat{
    static const int kProbeRecordCount = 300;

    struct ProbeRecord{
        uint64_t seq_num;
        uint64_t sent_timestamp;
        uint64_t received_timestamp;
    };
    ProbeRecord records[kProbeRecordCount];
    int next_position;
    uint64_t outdate_record_duration_ms = 3000;

    ProbeStat(uint64_t outdate_duration) {
        memset(records, 0, sizeof(records));
        next_position = 0;
        outdate_record_duration_ms = std::max(outdate_duration, static_cast<uint64_t>(1000));

    }
    void ProbeRequest(uint64_t seq_num, uint64_t ts) {
        records[next_position].seq_num = seq_num;
        records[next_position].sent_timestamp = ts;
        records[next_position].received_timestamp = 0;

        next_position = (next_position + 1) % kProbeRecordCount;
    }

    void ProbeResponse(uint64_t seq_num, uint64_t ts) {
        int previous_position = (next_position - 1 + kProbeRecordCount) % kProbeRecordCount;
        int delta = seq_num - records[previous_position].seq_num;
        delta = delta % kProbeRecordCount + kProbeRecordCount;
        int target_position = (previous_position + delta) % kProbeRecordCount;

        if (records[target_position].seq_num != seq_num) {
            return;
        }
        records[target_position].received_timestamp = ts;
    }

    std::pair<double, double> GetRttMeanAndStd(uint64_t ts) const {
        int cnt = 0;
        double total_time = 0;
        for (int i = 0; i < kProbeRecordCount; ++i) {
            if (ts - records[i].sent_timestamp > outdate_record_duration_ms 
                && records[i].received_timestamp != 0) {
                total_time += records[i].received_timestamp - records[i].sent_timestamp;
                cnt++;
            }

        }
        if (cnt == 0) {
            return {0, 0};
        }
        double rtt_mean = total_time / cnt;
        double square_sum = 0;
        for (int i = 0; i < kProbeRecordCount; ++i) {
            if (ts - records[i].sent_timestamp > outdate_record_duration_ms
                && records[i].received_timestamp != 0) {
                double v = records[i].received_timestamp - records[i].sent_timestamp - rtt_mean;
                square_sum = v * v;
            }
        }
        double rtt_std = sqrt(square_sum / cnt);

        return {rtt_mean, rtt_std};
    }
};

class LossStat{
private:
    static constexpr int report_duration_ms = 1000;
    static constexpr int BURST_LOST_SIZE = 6;
    static constexpr int GUARD_SPACE_SIZE = 10;
    struct Record{
        uint64_t second;
        uint32_t raw_packet_count;
        uint32_t raw_packet_loss_count;
        uint32_t data_packet_count;
        uint32_t data_packet_loss_count;

        std::pair<double, double> CalculateLoss() const {
            double raw_packet_loss = raw_packet_count == 0 ? 0 : (double)raw_packet_loss_count / raw_packet_count;
            double data_packet_loss = data_packet_count == 0 ? 0 : (double)data_packet_loss_count / data_packet_count;
            return {raw_packet_loss, data_packet_loss};
        }
        std::string ToString() const {
            std::stringstream ss;

            ss << "log_second: " << second << ", raw_packet_count: " << raw_packet_count << ", raw_packet_loss_count: " << raw_packet_loss_count \
                << ", data_packet_count: " << data_packet_count << ", data_packet_loss_count: " << data_packet_loss_count;
            auto loss = CalculateLoss();
            ss << ", raw_packet_loss: " << loss.first << ", data_packet_loss: " << loss.second; 
            ss << ", report_interval: " << (report_duration_ms / 1000) << "s";
            return ss.str();
        }
        Record& operator+=(const Record& rhs) {
            this->raw_packet_count += rhs.raw_packet_count;
            this->raw_packet_loss_count += rhs.raw_packet_loss_count;
            this->data_packet_count += rhs.data_packet_count;
            this->data_packet_loss_count += rhs.data_packet_loss_count;
            return *this;
        }
        friend std::ostream& operator<< (std::ostream& os, const Record& record) {
            os << record.ToString();
            return os;
        }
    };
    const static int kRecordLen = 60;
    Record data_[kRecordLen];
    uint64_t last_report_second_;
    std::vector<bool> received_status_;

    // 前6个表示burst loss, 后10个表示guard space， 大于等于10的合并在一起
    std::vector<int> GetBurstLossAndGuardSpace() {
        
        std::vector<int> ret(BURST_LOST_SIZE + GUARD_SPACE_SIZE, 0);
        auto calc_burst_index = [](int burst) {
            return std::min(BURST_LOST_SIZE, burst) - 1;
        };
        auto calc_guard_index = [](int guard) {
            return std::min(GUARD_SPACE_SIZE, guard) + BURST_LOST_SIZE - 1;
        };

        bool loss_happened_before = false;
        int burst_length = 0;
        int guard_space = 0;
        bool is_first_guard_space = true;
        bool is_first_burst_lost = true;
        for (int val : received_status_) {
            if (val == false) {
                if (loss_happened_before == false) {
                    if (guard_space) {
                        if (is_first_guard_space) {
                            is_first_guard_space = false;
                        }else {
                            ret[calc_guard_index(guard_space)]++;
                        }
                        guard_space = 0;
                    }
                    loss_happened_before = true;
                    burst_length ++;
                }else {
                    burst_length ++;
                }
            }
            else {
                if (loss_happened_before) {
                    if (burst_length) {
                        if (is_first_burst_lost) {
                            is_first_burst_lost = false;
                        } else {
                            ret[calc_burst_index(burst_length)]++;
                        }
                        burst_length = 0;
                    }
                    loss_happened_before = false;
                    guard_space += 1;
                }else {
                    guard_space += 1;
                }
            }
        }
        received_status_.clear();
        return ret;
    }
public:
    LossStat() {
        ClearState();
        last_report_second_ = 0;
        received_status_.reserve(100000);
    }
    void ClearState() {
        memset(data_, 0, sizeof data_);
    }
    void RegisterAPacket(uint64_t ts, bool received, bool processed, bool is_redundant) {
        uint64_t second = ts / 1000;
        received_status_.push_back(received);
        if (last_report_second_ == 0) {
            last_report_second_ = second;
        }else if (second > last_report_second_ && second - last_report_second_ >= (report_duration_ms / 1000)) {
            uint64_t i = last_report_second_;
            Record tmp{second, 0, 0, 0, 0};
            while (i != second) {
                int tmp_index = i % kRecordLen;
                DCHECK_EQ(data_[tmp_index].second, i);
                tmp += data_[tmp_index];
                ++i;
            }
            std::stringstream ss;
            ss << tmp;
            auto ret = GetBurstLossAndGuardSpace();
            int total_burst = 0;
            int total_guard = 0;
            for (int i = 0; i < BURST_LOST_SIZE; ++i) {
                total_burst += ret[i];
                std::string burst_length_str = ((i < BURST_LOST_SIZE - 1) ? 
                    (std::to_string(i + 1)) : (">=" + std::to_string(BURST_LOST_SIZE)));
                ss << ", burst length: " << burst_length_str << ", happended times: " << ret[i];
            }
            ss << ", total burst: " << total_burst;
            for (int i = BURST_LOST_SIZE; i < BURST_LOST_SIZE + GUARD_SPACE_SIZE; ++i) {
                total_guard += ret[i];
                std::string guard_space_str = ((i < BURST_LOST_SIZE + GUARD_SPACE_SIZE - 1) ?
                    (std::to_string(i - GUARD_SPACE_SIZE + 1)) : (">=") + std::to_string(GUARD_SPACE_SIZE));
                ss << ", guard space: " << guard_space_str << ", happended times: " << ret[i];
            }
            ss << ", total guard: " << total_guard;
            LOG(INFO) << ss.str();
            last_report_second_ = second;

        }
        int index = second % kRecordLen;
        if (data_[index].second != second) {
            data_[index].second = second;
            data_[index].raw_packet_count = 0;
            data_[index].raw_packet_loss_count = 0;
            data_[index].data_packet_count = 0;
            data_[index].data_packet_loss_count = 0;
        }
        data_[index].raw_packet_count++;
        data_[index].raw_packet_loss_count += (received ? 0 : 1);
        if (!is_redundant) { // Todo
            data_[index].data_packet_count++;
            data_[index].data_packet_loss_count += (processed ? 0 : 1);
        }
    }
    
    std::pair<double, double> GetLastMiniteLossRate(uint64_t ts) const {
        uint64_t start_second = ts / 1000 - 60;
        uint64_t end_second = start_second + 59;

        int total_raw_packet_count = 0;
        int total_raw_packet_loss_count = 0;
        int total_data_packet_count = 0;
        int total_data_packet_loss_count = 0;

        for (int i = 0; i < kRecordLen; ++i) {
            if (data_[i].second >= start_second && data_[i].second <= end_second) {
                total_raw_packet_count += data_[i].raw_packet_count;
                total_raw_packet_loss_count += data_[i].raw_packet_loss_count;
                total_data_packet_count += data_[i].data_packet_count;
                total_data_packet_loss_count += data_[i].data_packet_loss_count;
            }
        }
        double raw_packet_loss = total_raw_packet_count == 0 ? 0 : (double)total_raw_packet_loss_count / total_raw_packet_count;
        double data_packet_loss = total_data_packet_count == 0 ? 0 : (double)total_data_packet_loss_count / total_data_packet_count;
        return {raw_packet_loss, data_packet_loss};
    }

};


