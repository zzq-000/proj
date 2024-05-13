#pragma once
#include <fstream>
#include <vector>
#include "glog/logging.h"
#include "udp_socket.hh"
#include "FecCodec.h"
#include "packet.h"
#include "config.h"
class Receiver{
private:
    BlockFecCodec fec_codec;
    Config config;

public:
    Receiver(Config config): config(config) {

    };
    std::vector<Datagram> GetApplicationMessages(void* buffer, int len);

    ~Receiver() {};

};