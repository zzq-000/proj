#pragma once
#include <cstdint>
#include "util/timestamp.hh"
class FlowController {
private:
    uint64_t rate; // bytes/s
    uint64_t last_send_time_ms;
public:
    FlowController() {
        rate = 5 * 1024 * 1024; // 5MB/s
        last_send_time_ms = timestamp_ms();
    }
    FlowController(uint64_t speed) : rate(speed) {
        last_send_time_ms = timestamp_ms();
    }

    bool CanSend(int packet_size) {
        if (rate == 0) return true;
        auto now = timestamp_ms();

        uint64_t time_since_last_send = now - last_send_time_ms;
        
        
        uint64_t able_to_send = rate / 1000 * time_since_last_send;

        
        // Check if enough time has elapsed since the last send
        if (able_to_send >= packet_size) {
            // Update the last send time
            last_send_time_ms = now;
            return true;
        } else {
            return false;
        }
    }
};