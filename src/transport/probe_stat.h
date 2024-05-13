#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cmath>
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
            if (ts - records[i].sent_timestamp > outdate_record_duration_ms) {
                continue;
            }
            total_time += records[i].received_timestamp - records[i].sent_timestamp;
            cnt++;

        }
        double rtt_mean = total_time / cnt;
        double square_sum = 0;
        for (int i = 0; i < kProbeRecordCount; ++i) {
            if (ts - records[i].sent_timestamp > outdate_record_duration_ms) {
                continue;
            }
            double v = records[i].received_timestamp - records[i].sent_timestamp - rtt_mean;
            square_sum = v * v;
        }
        double rtt_std = sqrt(square_sum / cnt);

        return {rtt_mean, rtt_std};
    }
};