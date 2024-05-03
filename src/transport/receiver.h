#include <fstream>
#include <vector>
#include "glog/logging.h"
#include "udp_socket.hh"
#include "FecCodec.h"
#include "Packet.h"
class Receiver{
private:
    Address server_address;
    UDPSocket sock_fd;
    BlockFecCodec fec_codec;
    std::ofstream file;
    Fec_type fec_type;
    std::string file_name;
    bool has_send;
public:
    Receiver(std::string filename, Fec_type type, Address& address)
        : file_name(filename), fec_type(type), server_address(address), has_send(false) {
        file.open(filename);
        sock_fd.bind(address);
        if (file.fail()) {
            LOG(FATAL) << "could not open the file: " << filename;
        }
    };
    void KeepReceive() {
        while (true) {
            auto [Address, message] = sock_fd.recvfrom();
            Packet* data = (Packet*)(message.value().data());

            std::string s(data->GetContent(), data->GetContent() + data->GetContentLen());
            // file << s;
            if (data->IsEOF()) {
                file.close();
                break;
            }
            LOG(INFO) << "received a packet, sequence: " << data->GetSequenceNum() << "; size: " << data->PacketSize();
        }
    }
    void Receive() {
        while (true)
        {
            auto [Address, message] = sock_fd.recvfrom();
            Packet* data = (Packet*)(message.value().data());

            std::string s(data->GetContent(), data->GetContent() + data->GetContentLen());
            file << s;
            if (data->IsEOF()) {
                file.close();
                break;
            }

        }
        LOG(INFO) << "successfully received a file";
        
    }
    ~Receiver() {};

};