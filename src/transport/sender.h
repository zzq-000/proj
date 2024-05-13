# pragma once
#include <fstream>
#include <vector>
#include <list>
#include "glog/logging.h"
#include "udp_socket.hh"
#include "FecCodec.h"
#include "packet.h"
#include "flow_controller.h"
#include "packet_buffer.h"
#include "config.h"
#include "timestamp.hh"
class Sender{
private:
    BlockFecCodec fec_codec;
    Config config;
    FlowController fc;
    PacketBuffer& packets;
    uint64_t seq_num = 0;
    std::list<pair<int, uint64_t>> packets_to_send; 
    
    
    std::vector<Datagram> HandleFecPackets(uint64_t ts);
    std::vector<Datagram> HandleFecTypeNonePackets(uint64_t ts);
    void InitPacket(Packet* p);
public:
    Sender() = delete;
    Sender(Config cfg, PacketBuffer& packet_buffer): config(cfg), packets(packet_buffer) { }
    std::vector<Datagram> RegisterMessages(void* buffer, int len);
    ~Sender() {};

};