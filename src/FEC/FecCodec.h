#pragma once
#include <vector>
#include <glog/logging.h>
#include "FecDatagram.h"
#include "cm256.h"
class FecCodec{

public:
    virtual bool Encode(std::vector<void*>& buffers, FecType type, int encode_length) = 0;
    virtual bool Decode(std::vector<void*>& buffers, FecType type, int decode_length) = 0;

    virtual ~FecCodec() {};
};

class BlockFecCodec : public FecCodec
{
private:
    cm256_block blocks[256];
    cm256_encoder_params params;
public:
    BlockFecCodec() {
        if (cm256_init()) {
            LOG(ERROR) << "init error";
        }
    };
    bool Encode(std::vector<void*>& buffers, FecType type, int encode_length) override;
    bool Decode(std::vector<void*>& buffers, FecType type, int decode_length) override;
    ~BlockFecCodec() {};
};