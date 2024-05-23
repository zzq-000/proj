
#include "FEC/fec_type.pb.h"
struct Config{

    bool enable_nack = true;
    
    FecType fec_type = FecType::FEC_2_1;

    uint64_t fec_encode_wait_time_limit_ms = 40;
};