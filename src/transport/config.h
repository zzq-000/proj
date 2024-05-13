
#include "FecCodec.h"

struct Config{

    bool enable_nack = true;
    bool enable_fec  = true;
    
    FecType fec_type = FecType::FEC_2_1;

    uint64_t fec_encode_wait_time_limit_ms = 40;
};