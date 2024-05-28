#pragma once
#include "FEC/fec_type.pb.h"

constexpr int kMTU = 1500;
constexpr int kUDPHeader = 8;
constexpr int kIPHeader = 20;
constexpr int KMaxPayloadLen = kMTU - kUDPHeader - kIPHeader;

struct Config{

    bool enable_nack;
    
    FecType fec_type;

    uint64_t fec_encode_wait_time_limit_ms;
    uint64_t fec_decode_wait_time_limit_ms;


    static Config GetDefaultConfig() {
        Config config;
        
        config.enable_nack = true;

        config.fec_type = FEC_NONE;
        
        config.fec_encode_wait_time_limit_ms = 40;
        config.fec_decode_wait_time_limit_ms = 60;
        
        return config;
    }
};