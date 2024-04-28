#pragma once
#include <vector>
#include <glog/logging.h>
#include "FecDatagram.h"
#include "cm256.h"
class FecCodec{

public:
    virtual void Encode(std::vector<FecDatagram*>& buffers, Fec_type type) = 0;
    virtual bool Decode(std::vector<FecDatagram*>& buffers, Fec_type type) = 0;

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
    void Encode(std::vector<FecDatagram*>& buffers, Fec_type type) override;
    bool Decode(std::vector<FecDatagram*>& buffers, Fec_type type) override;
    ~BlockFecCodec() {};
};