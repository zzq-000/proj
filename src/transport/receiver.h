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
    // void SendTo(Address address) {
    //     if (file.eof()) {
    //         file.clear();
    //         file.open(file_name);
    //     }
    //     std::vector<char> contents((std::istreambuf_iterator<char>(file)),
    //                                 std::istreambuf_iterator<char>());
    //     file.close();
    //     has_send = true;

    //     std::vector<Packet> datagrams = Packet::GeneratePackets<char>(contents.data(), contents.size());

    //     for (int i = 0; i < datagrams.size(); ++i) {
    //         sock_fd.sendto(server_address, (char*)&datagrams[i]);
    //     }
    // }

    ~Receiver() {};

};