#include <fstream>
#include <vector>
#include "glog/logging.h"
#include "udp_socket.hh"
#include "FecCodec.h"
#include "Packet.h"
class Sender{
private:
    Address server_address;
    UDPSocket sock_fd;
    BlockFecCodec fec_codec;
    std::ifstream file;
    Fec_type fec_type;
    std::string file_name;
    bool has_send;
public:
    Sender(std::string filename, Fec_type type, Address& address)
        : file_name(filename), fec_type(type), server_address(address), has_send(false) {
        file.open(filename);
        if (file.fail()) {
            LOG(FATAL) << "could not open the file: " << filename;
        }
    };

    void SendTo(Address address) {
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
            
            
          ;
        }
    }

    ~Sender() {};

};