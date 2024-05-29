#pragma once
#include "FEC/fec_type.pb.h"

constexpr int kMTU = 1500;
constexpr int kUDPHeader = 8;
constexpr int kIPHeader = 20;
constexpr int kMaxPayloadLen = kMTU - kUDPHeader - kIPHeader;
constexpr int kMaxFecTotalPackets = 20;
struct Config{

    bool enable_nack;
    
    FecType fec_type;

    uint64_t fec_encode_wait_time_limit_ms;
    uint64_t fec_decode_wait_time_limit_ms;
    uint64_t nack_trigger_wait_time_limit_ms;

    static Config GetDefaultConfig() {
        Config config;
        
        config.enable_nack = true;

        config.fec_type = FEC_NONE;
        
        config.fec_encode_wait_time_limit_ms = 40;

        //发现该包组超过30ms还没有接收完全， 则触发重传
        config.nack_trigger_wait_time_limit_ms = 30;

        // 超过多少毫秒， 不管该包组是否接收完全， 都要往应用层交付
        config.fec_decode_wait_time_limit_ms = 60;
        
        return config;
    }
};