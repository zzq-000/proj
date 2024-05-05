# pragma once
#include <fstream>
#include <vector>
#include "glog/logging.h"
#include "udp_socket.hh"
#include "FecCodec.h"
#include "Packet.h"
#include "flow_controller.h"
#include "PacketBuffer.h"

class Sender{
private:
    Address server_address;
    UDPSocket sock_fd;
    BlockFecCodec fec_codec;
    std::ifstream file;
    Fec_type fec_type;
    std::string file_name;
    bool has_send;
    FlowController fc;
    PacketBuffer buffer;
public:
    Sender():server_address("127.0.0.1", 8000) {
        has_send = false;

    }
    Sender(std::string filename, Fec_type type, Address& address, int rate)
        : file_name(filename), fec_type(type), server_address(address), has_send(false), fc(rate) {
        file.open(filename);
        if (file.fail()) {
            LOG(ERROR) << "could not open the file: " << filename;
        }
    };
    void KeepSend(int start_seq) {
        // int start_seq = rand() % 1000 + 10;
        Packet p = Packet::GenerateRandomPacket();
        // p.SetStart();
        p.SetSequenceNum(start_seq++);
        uint64_t log_time_ms = 0;
        uint64_t kLogDuration = 60;
        do {
            if (fc.CanSend(p.PacketSize())) {
                sock_fd.sendto(server_address, (void*)&p, p.PacketSize());
                LOG(INFO) << "send a packet, seqence: " << p.GetSequenceNum() << "; size: " << p.PacketSize()
                    << "; to: " << server_address.str();
                
                p = Packet::GenerateRandomPacket();
                buffer.CacheAPacket(p);
                p.SetSequenceNum(start_seq++);
                if (timestamp_ms() - log_time_ms > 1000) {
                    LOG(INFO) << "buffer size: " << buffer.Size();
                    log_time_ms = timestamp_ms();
                }
            }
        }while(true);
        

    }
    void SendTo() {
        if (file.eof()) {
            file.clear();
            file.open(file_name);
        }
        std::vector<char> contents((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
        // std::cout << contents.size() << std::endl;
        LOG(INFO) << "filesize: " << contents.size();
        file.close();
        has_send = true;

        std::vector<Packet> datagrams = Packet::GeneratePackets<char>(contents.data(), contents.size());
        LOG(INFO) << datagrams.size();
        for (int i = 0 ; i< datagrams.size(); ++i) {
            LOG(INFO) << i << ": " << datagrams[i].GetContentLen();
        }
        // for (int i = 0;)
        // std::string_view v();
        for (int i = 0; i < datagrams.size(); ++i) {
            sock_fd.sendto(server_address, &datagrams[i], sizeof(Packet));
        }
    }

    ~Sender() {};

};